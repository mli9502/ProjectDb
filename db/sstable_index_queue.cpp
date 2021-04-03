//
// Created by Mengwen Li (ml4643) on 4/2/2021.
//

#include "sstable_index_queue.h"

#include <future>

#include "db_config.h"
#include "log.h"
#include "sstable_ops.h"

namespace projectdb {

SSTableIndexQueue::SSTableIndexQueue() : m_compactionStartIndex(0) {}

optional<Table::mapped_type> SSTableIndexQueue::get(string_view key) {
    Table::key_type tableKey{string{key}};

    for (auto it = m_queue.rbegin(); it != m_queue.rend(); it++) {
        const auto& seekRtn = it->seek(tableKey);
        if (seekRtn.has_value()) {
            return seekRtn;
        }
    }

    return {};
}

optional<future<vector<SSTableIndex>>> SSTableIndexQueue::insert(
    SSTableIndex&& ssTableIndex) {
    m_queue.emplace_back(move(ssTableIndex));
    return tryLaunchCompaction();
}

void SSTableIndexQueue::update(
    vector<SSTableIndex>&& ssTableIndexAfterCompaction) {
    if (db_config::NUM_SSTABLE_TO_COMPACT <= 0) {
        log::errorAndThrow("Invalid NUM_SSTABLE_TO_COMPACT for update!");
    }
    // First remove the deprecated SSTableIndexs
    if (distance(m_queue.begin() + m_compactionStartIndex + 1, m_queue.end()) <
        db_config::NUM_SSTABLE_TO_COMPACT) {
        log::errorAndThrow("SSTableIndex queue size does not look right!");
    }
    if (ssTableIndexAfterCompaction.empty()) {
        log::errorAndThrow(
            "Got empty ssTableIndexAfterCompaction. This should not happen "
            "when NUM_SSTABLE_TO_COMPACT > 0!");
    }
    auto eraseBeginIt = m_queue.begin() + m_compactionStartIndex;
    auto eraseEndIt = m_queue.begin() + m_compactionStartIndex + 1 +
                      db_config::NUM_SSTABLE_TO_COMPACT;
    // Mark all the SSTables that we have compressed as deprecated.
    for_each(eraseBeginIt, eraseEndIt, [](const auto& ssTableIndex) {
        markFileAsDeprecated(ssTableIndex.getSSTableFileName());
    });
    m_queue.erase(m_queue.begin() + m_compactionStartIndex,
                  m_queue.begin() + m_compactionStartIndex + 1 +
                      db_config::NUM_SSTABLE_TO_COMPACT);

    // Rename all merged SSTable files to normal SSTable files.
    for_each(ssTableIndexAfterCompaction.begin(),
             ssTableIndexAfterCompaction.end(), [](auto& ssTableIndex) {
                 ssTableIndex.setSSTableFileName(markMergedSSTableFileAsActive(
                     ssTableIndex.getSSTableFileName()));
             });

    // The updated compactionStartIndex points to the index that represents the
    // last element of ssTableIndexAfterCompaction after inserting them into
    // queue. This is because it's possible that the last compacted SSTable has
    // not reach it's max size.
    auto updatedCompactionStartIndex =
        m_compactionStartIndex + ssTableIndexAfterCompaction.size() - 1;
    // https://stackoverflow.com/questions/10720122/is-there-a-standard-way-of-moving-a-range-into-a-vector
    m_queue.insert(m_queue.begin() + m_compactionStartIndex,
                   make_move_iterator(ssTableIndexAfterCompaction.begin()),
                   make_move_iterator(ssTableIndexAfterCompaction.end()));
    log::debug("Updating compationStartIndex from ", m_compactionStartIndex,
               " to ", updatedCompactionStartIndex);
    m_compactionStartIndex = updatedCompactionStartIndex;
}

optional<future<vector<SSTableIndex>>>
SSTableIndexQueue::tryLaunchCompaction() {
    if (db_config::NUM_SSTABLE_TO_COMPACT <= 0) {
        log::info("NUM_SSTABLE_TO_COMPACT <= 0, compaction is disabled.");
        return {};
    }
    auto newlyAddedSSTablesCnt = m_queue.size() - m_compactionStartIndex - 1;
    log::debug("Newly added SSTable count is: ", newlyAddedSSTablesCnt);
    if (newlyAddedSSTablesCnt < db_config::NUM_SSTABLE_TO_COMPACT) {
        log::debug("Will not start compaction.");
        return {};
    }
    log::debug("Starting compaction...");
    return async(launch::async, [&]() {
        return mergeSSTables(m_queue.begin() + m_compactionStartIndex,
                             m_queue.begin() + m_compactionStartIndex + 1 +
                                 db_config::NUM_SSTABLE_TO_COMPACT);
    });
}

}  // namespace projectdb

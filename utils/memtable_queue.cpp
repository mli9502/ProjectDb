//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "memtable_queue.h"

#include <algorithm>
#include <thread>

#include "log.h"
#include "sstable_index.h"

using namespace std;

namespace projectdb {

MemTableQueue::MemTableQueue() { m_queue.push_back(make_shared<MemTable>()); }

optional<string> MemTableQueue::get(string_view key) const {
    MemTable::key_type tableKey{string(key)};
    const auto cit =
        find_if(m_queue.crbegin(), m_queue.crend(), [&](const auto& memTable) {
            return memTable->getValueEntry(tableKey).has_value();
        });
    if (cit == m_queue.crend()) {
        return {};
    }
    return (*cit)->getValue(tableKey);
}

optional<future<SSTableIndex>> MemTableQueue::set(string_view key,
                                                  string_view value) {
    m_queue.back()->set(MemTable::key_type{string(key)},
                        MemTable::mapped_type{string(value)});
    return tryLaunchFlushToDisk(*(m_queue.back()));
}

optional<future<SSTableIndex>> MemTableQueue::remove(string_view key) {
    m_queue.back()->remove(MemTable::key_type{string(key)});
    return tryLaunchFlushToDisk(*(m_queue.back()));
}

void MemTableQueue::pop() { m_queue.pop_front(); }

optional<future<SSTableIndex>> MemTableQueue::tryLaunchFlushToDisk(
    const MemTable& memTable) {
    // TODO: @mli: Need to update this to memTable.sizeFull(). Also need to pass
    // max size as parameter.
    if (memTable.getApproximateSizeInBytes() <
        MemTable::APPROXIMATE_MAX_SIZE_IN_BYTES) {
        return {};
    }
    log::debug("Size exceeds ", MemTable::APPROXIMATE_MAX_SIZE_IN_BYTES,
               " bytes. Adding a new MemTable to queue and start async job to "
               "flush memtable to disk...");
    m_queue.push_back(make_shared<MemTable>());
    return async(launch::async, [&]() {
        log::debug(
            "Build SSTable, flush it to disk and build SSTableIndex from "
            "memTable...");
        // TODO: @mli: Implement this. And add sleep for now for testing.
        return SSTableIndex();
    });
}

}  // namespace projectdb

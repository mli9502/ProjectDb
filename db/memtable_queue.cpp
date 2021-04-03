//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "memtable_queue.h"

#include <algorithm>
#include <future>

#include "log.h"
#include "sstable.h"
#include "sstable_index.h"
#include "sstable_ops.h"

using namespace std;

namespace projectdb {
// NOTE: @mli: We can't just return optional<string> because it's possible that
// we get a TOMBSTONE here, in this case, we should just finish instead of
// continue searching in SSTableIndex.
/**
 * Search through the queue for a given key.
 * @param key
 * @return
 */
optional<MemTable::mapped_type> MemTableQueue::get(const string& key) const {
    if (m_queue.empty()) {
        return {};
    }
    MemTable::key_type tableKey{key};
    const auto cit =
        find_if(m_queue.crbegin(), m_queue.crend(), [&](const auto& entry) {
            return entry.first.getValue(tableKey).has_value();
        });
    if (cit == m_queue.crend()) {
        return {};
    }
    return cit->first.getValue(tableKey);
}

/**
 * Set the key value pair in the latest MemTable in the queue.
 */
optional<future<SSTableIndex>> MemTableQueue::set(const string& key,
                                                  const string& value) {
    if (m_queue.empty()) {
        m_queue.emplace_back();
    }
    m_queue.back().second.write(DbTransactionType::SET, key, value);
    m_queue.back().first.set(MemTable::key_type{key},
                             MemTable::mapped_type{value});
    return tryLaunchFlushToDisk(m_queue.back().first);
}

/**
 * Set the value corresponding to the given key
 * in the latest MemTable in the queue.
 */
optional<future<SSTableIndex>> MemTableQueue::remove(const string& key) {
    if (m_queue.empty()) {
        m_queue.emplace_back();
    }
    m_queue.back().second.write(DbTransactionType::REMOVE, key);
    m_queue.back().first.remove(MemTable::key_type{key});
    return tryLaunchFlushToDisk(m_queue.back().first);
}

void MemTableQueue::pop() { m_queue.pop_front(); }

/**
 * See if the provided MemTable needs to be flush to disk
 * and tries to if needed.
 */
optional<future<SSTableIndex>> MemTableQueue::tryLaunchFlushToDisk(
    const MemTable& memTable) {
    if (!memTable.needsFlushToDisk()) {
        return {};
    }
    log::debug(
        "MemTable needs to be flushed to disk. Adding a new MemTable to queue "
        "and start async job to flush current MemTable to disk...");
    m_queue.emplace_back();
    return async(launch::async, [&]() {
        log::debug(
            "Build SSTable, flush it to disk and build SSTableIndex from "
            "memTable...");

        return flushSSTable(SSTable(memTable.getTable()), genSSTableFileName());
        // TODO: @mli: Add code in the place that handles async return to mark
        // transaction_log as deprecated.
    });
}

}  // namespace projectdb

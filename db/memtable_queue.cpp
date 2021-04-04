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

MemTableQueue::MemTableQueue() { m_queue.emplace_back(); }

// NOTE: @mli: We can't just return optional<string> because it's possible that
// we get a TOMBSTONE here, in this case, we should just finish instead of
// continue searching in SSTableIndex.
/**
 * Search through the queue for a given key.
 * @param key
 * @return
 */
optional<MemTable::mapped_type> MemTableQueue::get(string_view key) const {
    MemTable::key_type tableKey{string{key}};
    const auto cit =
        find_if(m_queue.crbegin(), m_queue.crend(), [&](const auto& memTable) {
            return memTable.getValue(tableKey).has_value();
        });
    if (cit == m_queue.crend()) {
        return {};
    }
    return cit->getValue(tableKey);
}

/**
 * Set the key value pair in the latest MemTable in the queue.
 */
optional<future<SSTableIndex>> MemTableQueue::set(string_view key,
                                                  string_view value) {
    m_queue.back().set(MemTable::key_type{string(key)},
                       MemTable::mapped_type{string(value)});
    return tryLaunchFlushToDisk(m_queue.back());
}

/**
 * Set the value corresponding to the given key
 * in the latest MemTable in the queue.
 */
optional<future<SSTableIndex>> MemTableQueue::remove(string_view key) {
    m_queue.back().remove(MemTable::key_type{string(key)});
    return tryLaunchFlushToDisk(m_queue.back());
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

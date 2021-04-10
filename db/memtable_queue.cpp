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
// NOTE: @mli:
// We have to return optional<Value> instead of optional<string> (aka
// optional<Value::value_type>) because it's possible that we get a TOMBSTONE
// here, in this case, we should just finish instead of continue searching in
// SSTableIndex.
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

optional<future<SSTableIndex>> MemTableQueue::pushFromTransactionLog(
    const string& transactionLogFileName, bool isLastTransactionLog) {
    m_queue.emplace_back(TransactionLogLoader::load(transactionLogFileName),
                         TransactionLogWritter(transactionLogFileName));
    if (!isLastTransactionLog) {
        // NOTE: @mli:
        // In here, we don't call tryLaunchFlushToDisk,
        // because if this is not the last transaction log (which we might still
        // be able to write to), we are sure that we need the flush, so we don't
        // need to check needsFlushToDisk. Also, since when it's not the last
        // loaded MemTable, we are just cleaning up our previous runs, so we
        // don't want to add a new MemTable entry at the end of the queue.
        return launchFlushToDisk(m_queue.back().first);
    }
    return {};
}

void MemTableQueue::pop() {
    auto transactionLogFileName =
        m_queue.front().second.getTransactionLogFileName();
    m_queue.pop_front();
    markFileAsDeprecated(transactionLogFileName);
}

/**
 * See if the provided MemTable needs to be flush to disk
 * and tries to if needed.
 */
optional<future<SSTableIndex>> MemTableQueue::tryLaunchFlushToDisk(
    const MemTable& memTable) {
    if (!memTable.needsFlushToDisk()) {
        return {};
    }
    m_queue.emplace_back();
    return launchFlushToDisk(memTable);
}

future<SSTableIndex> MemTableQueue::launchFlushToDisk(
    const MemTable& memTable) {
    log::debug("Launch job to flush MemTable to disk.");
    auto currTable = memTable.getTable();
    unsigned currSSTableFileCounter =
        ++db_config::impl::SSTABLE_FILE_COUNTER_BASE;
    // NOTE: @mli:
    // In here we CAN'T capture by reference, because
    // currSSTableFileCounter will be destructed as soon as thread launches,
    // resulting in a garbage value.
    return async(launch::async, [currTable, currSSTableFileCounter]() {
        return flushSSTable(SSTable(currTable),
                            genFlushInProgressSSTableFileName(
                                genSSTableFileName(currSSTableFileCounter)));
    });
}

}  // namespace projectdb

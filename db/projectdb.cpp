//
// Created by Mengwen Li (ml4643) on 4/3/2021.
//

#include "projectdb/projectdb.h"

#include "log.h"
#include "memtable_queue.h"
#include "sstable_index_queue.h"

using namespace std;

namespace projectdb {

class ProjectDbImpl {
   public:
    optional<string> get(const string& key);
    void set(const string& key, const string& value);
    void remove(const string& key);

   private:
    MemTableQueue m_memTableQueue;
    SSTableIndexQueue m_ssTableIndexQueue;

    // [old ... new]
    vector<future<SSTableIndex>> m_flushToDiskFutures;
    optional<future<vector<SSTableIndex>>> m_ssTableCompactionFuture;

    void init();

    void checkFutures();
    void checkFlushToDiskFutures();
    bool checkSSTableCompactionFuture();
};

optional<string> ProjectDbImpl::get(const string& key) {
    checkFutures();

    const auto& memTableRtn = m_memTableQueue.get(key);
    if (memTableRtn.has_value()) {
        log::info("Found key: [", key, "] in MemTableQueue with Value: [",
                  memTableRtn.value(), "]");
        return memTableRtn.value().underlyingValue();
    }

    const auto& ssTableRtn = m_ssTableIndexQueue.get(key);
    if (ssTableRtn.has_value()) {
        log::info("Found key: [", key, "] in SSTableIndexQueue with Value: [",
                  ssTableRtn.value(), "]");
        return ssTableRtn.value().underlyingValue();
    }

    log::info("Key: [", key, "] not found.");
    return {};
}

void ProjectDbImpl::set(const string& key, const string& value) {
    checkFutures();

    auto&& ft = m_memTableQueue.set(key, value);
    if (ft.has_value()) {
        log::debug("Starting flushToDisk job.");
        m_flushToDiskFutures.emplace_back(move(ft.value()));
    }
}

void ProjectDbImpl::remove(const string& key) {
    checkFutures();

    auto&& ft = m_memTableQueue.remove(key);
    if (ft.has_value()) {
        log::debug("Starting flushToDisk job.");
        m_flushToDiskFutures.emplace_back(move(ft.value()));
    }
}

// TODO: @mli: We might need to first write SSTable to SSTable.in_progress, and
// update its file name to remove the suffix when it's flushed. By doing this,
// we can know during initialization which SSTable should we pick up.
/**
 * Performs the following operations to initialize the database:
 * 1. Remove all files with .deprecated and .merged suffix, since they are temp
 * files. 2.
 */
void ProjectDbImpl::init() {}

void ProjectDbImpl::checkFutures() {
    bool hasCompactionRunning = checkSSTableCompactionFuture();
    // When a compaction is running in a separate thread, we CAN'T call
    // checkFlushToDiskFutures. This is because both
    // checkFlushToDiskFutures, and compaction will update
    // m_ssTableIndexQueue. Calling checkFlushToDiskFutures while compaction
    // is running will cause a race condition, and will lead to iterators
    // being invalidated. This means that we will not flush MemTable to disk
    // during compaction.
    if (hasCompactionRunning) {
        log::debug(
            "Compaction is running, will not try checkFlushToDiskFutures.");
    } else {
        checkFlushToDiskFutures();
    }
}

/**
 * Go through m_flushToDiskFutures to process the finished ones.
 * NOTE: @mli:
 * We start from the beginning (oldest one), and keep processing it and stop
 * when we reach one that is not finished. We should not try to process the
 * others after the not finished one, since this might cause
 * SSTableIndexQueue to contain out-of-order SSTableIndex.
 */
void ProjectDbImpl::checkFlushToDiskFutures() {
    log::debug("In checkFlushToDiskFutures");
    auto it = m_flushToDiskFutures.begin();
    while (it != m_flushToDiskFutures.end()) {
        if (it->wait_for(chrono::seconds(0)) != future_status::ready) {
            break;
        }
        auto ssTableIndex = it->get();
        log::debug("Got SSTableIndex for SSTable: ",
                   ssTableIndex.getSSTableFileName());
        m_ssTableIndexQueue.insert(move(ssTableIndex));
        // TODO: @mli: In here, mark the SSTable as active by removing the
        // inprogress suffix. Remove the corresponding MemTable and
        // TransactionLog.
        m_memTableQueue.pop();
        it = m_flushToDiskFutures.erase(it);
    }
    // NOTE: @mli: Compaction is launched only when all the updates to
    // m_ssTableIndexQueue are done. Since both this method, and
    // tryLaunchCompaction will modify m_ssTableIndexQueue, we have to
    // guarantee that the queue is only updated by one of these at any given
    // time, otherwise, the pointer will be invalidated.
    // Also note that we can start the flushToDisk job, since it just
    // returns the SSTableIndex without modifying the ssTableIndexQueue.
    // However, we can't call checkFlushToDiskFutures while compaction job
    // is running, since checkFlushToDiskFutures will update the queue.
    log::debug("Try starting SSTable compaction job.");
    auto&& ft = m_ssTableIndexQueue.tryLaunchCompaction();
    if (ft.has_value()) {
        log::debug("Starting SSTable compaction job.");
        m_ssTableCompactionFuture = move(ft.value());
    }
    log::debug("checkFlushToDiskFutures done.");
}

/**
 * Only one compaction can be done at a single time!
 * This is because new compaction depends on the result of previous
 * compaction, since the new SSTables might be compacted into the last one
 * from previous compaction.
 * Returns true if there's currently a compaction job running.
 */
bool ProjectDbImpl::checkSSTableCompactionFuture() {
    if (!m_ssTableCompactionFuture.has_value()) {
        return false;
    }
    if (m_ssTableCompactionFuture.value().wait_for(chrono::seconds(0)) !=
        future_status::ready) {
        return true;
    }
    auto ssTableIndexAfterCompaction = m_ssTableCompactionFuture.value().get();
    m_ssTableIndexQueue.update(move(ssTableIndexAfterCompaction));
    m_ssTableCompactionFuture = {};
    return false;
}

ProjectDb::ProjectDb() : m_impl(make_unique<ProjectDbImpl>()) {}
ProjectDb::~ProjectDb() = default;

optional<string> ProjectDb::get(const string& key) { return m_impl->get(key); }
void ProjectDb::set(const string& key, const string& value) {
    return m_impl->set(key, value);
}
void ProjectDb::remove(const string& key) { return m_impl->remove(key); }

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 4/3/2021.
//

#include "projectdb/projectdb.h"

#include "config_parser.h"
#include "log.h"
#include "memtable_queue.h"
#include "sstable_index_queue.h"
#include "sstable_ops.h"
#include "system_utils.h"

using namespace std;

namespace projectdb {

class ProjectDbImpl {
   public:
    ProjectDbImpl(const string& configFilePath);
    ~ProjectDbImpl();
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

ProjectDbImpl::ProjectDbImpl(const string& configFilePath) {
    if (!configFilePath.empty()) {
        ConfigParser::parse(configFilePath);
    }
    init();
}

/**
 * In dtor, wait for all flushToDisk and ssTableCompaction jobs to be done
 * before finish.
 */
ProjectDbImpl::~ProjectDbImpl() {
    log::debug("Finishing up in-progress jobs before closing ProjectDb...");
    // NOTE: @mli:
    // checkFlushToDiskFutures is not called in here because we just
    // want to finish all flushToDisk, and don't want to start any more
    // compactions.
    log::debug("Waiting for flushToDisk jobs to finish...");
    for (auto& ft : m_flushToDiskFutures) {
        auto ssTableIndex = ft.get();
        log::debug("Got SSTableIndex for SSTable: ",
                   ssTableIndex.getSSTableFileName());
        removeExtAndRename(ssTableIndex.getSSTableFileName());
        // Remove the corresponding MemTable and TransactionLog.
        m_memTableQueue.pop();
    }
    log::debug("Waiting for SSTable compaction job to finish...");
    while (checkSSTableCompactionFuture()) {
    }
    // Finially removing all deprecated files.
    log::debug("Removing deprecated files...");
    removeFilesWithExt(db_config::impl::DEPRECATED_FILE_EXT);
    log::debug("Done.");
}

optional<string> ProjectDbImpl::get(const string& key) {
    checkFutures();

    const auto& memTableRtn = m_memTableQueue.get(key);
    if (memTableRtn.has_value()) {
        log::debug("Found key: [", key, "] in MemTableQueue with Value: [",
                   memTableRtn.value(), "]");
        return memTableRtn.value().underlyingValue();
    }

    const auto& ssTableRtn = m_ssTableIndexQueue.get(key);
    if (ssTableRtn.has_value()) {
        log::debug("Found key: [", key, "] in SSTableIndexQueue with Value: [",
                   ssTableRtn.value(), "]");
        return ssTableRtn.value().underlyingValue();
    }

    log::debug("Key: [", key, "] not found.");
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

/**
 * Performs the following operations to initialize the database:
 * 1. Remove all files with .deprecated, .merged and .ip suffix, since they are
 * temp files.
 * 2. Load .sst files and generate SSTableIndex, update
 * SSTABLE_FILE_COUNTER_BASE to <last .sst counter> + 1. Note that we need to
 * load SSTables before MemTables, this is because SSTable represents the
 * entries that are added to db earlier (old data). Also, when loading
 * MemTables, it's possible that we need to flush some MemTables to disk, so we
 * need SSTABLE_FILE_COUNTER_BASE to be properly set.
 * 3. Load .txl files as MemTable, launch async job to flush MemTables to disk
 * besides the last one if more than one MemTables are loaded from disk. Update
 * TRANSACTION_LOG_FILE_COUNTER_BASE to <last .txl counter> + 1.
 *
 */
void ProjectDbImpl::init() {
    initDbPath();

    log::debug("Removing ", db_config::impl::DEPRECATED_FILE_EXT, " files...");
    removeFilesWithExt(db_config::impl::DEPRECATED_FILE_EXT);
    log::debug("Removing ", db_config::impl::SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT,
               " files...");
    removeFilesWithExt(db_config::impl::SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT);
    log::debug("Removing ", db_config::impl::MERGED_SSTABLE_FILE_EXT,
               " files...");
    removeFilesWithExt(db_config::impl::MERGED_SSTABLE_FILE_EXT);

    log::debug("Reloading SSTables from disk...");
    auto ssTableFiles =
        getFilesWithExtSorted(db_config::impl::SSTABLE_FILE_EXT);
    for_each(ssTableFiles.cbegin(), ssTableFiles.cend(),
             [&](const auto& ssTableFileName) {
                 log::debug("Processing SSTable file: ", ssTableFileName);
                 SSTableIndex ssTableIndex;
                 loadSSTable(ssTableFileName, &ssTableIndex);
                 m_ssTableIndexQueue.insert(move(ssTableIndex));
             });

    if (!ssTableFiles.empty()) {
        db_config::impl::SSTABLE_FILE_COUNTER_BASE =
            1 + getCounterFromFileName(ssTableFiles.back());
        log::debug("SSTABLE_FILE_COUNTER_BASE updated to: ",
                   db_config::impl::SSTABLE_FILE_COUNTER_BASE);
    }

    log::debug("Reloading Transaction Logs from disk...");
    auto transactionLogFiles =
        getFilesWithExtSorted(db_config::impl::TRANSACTION_LOG_FILE_EXT);
    for (auto cit = transactionLogFiles.cbegin();
         cit != transactionLogFiles.end(); cit++) {
        bool isLastTransactionLog = (next(cit) == transactionLogFiles.end());
        auto&& ft =
            m_memTableQueue.pushFromTransactionLog(*cit, isLastTransactionLog);
        if (ft.has_value()) {
            m_flushToDiskFutures.emplace_back(move(ft.value()));
        }
    }

    if (!transactionLogFiles.empty()) {
        db_config::impl::TRANSACTION_LOG_FILE_COUNTER_BASE =
            1 + getCounterFromFileName(transactionLogFiles.back());
        log::debug("TRANSACTION_LOG_FILE_COUNTER_BASE updated to: ",
                   db_config::impl::TRANSACTION_LOG_FILE_COUNTER_BASE);
    }
}

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
        ssTableIndex.setSSTableFileName(
            removeExtAndRename(ssTableIndex.getSSTableFileName()));
        m_ssTableIndexQueue.insert(move(ssTableIndex));
        // Remove the corresponding MemTable and TransactionLog.
        m_memTableQueue.pop();
        it = m_flushToDiskFutures.erase(it);
    }
    // NOTE: @mli:
    // Compaction is launched only when all the updates to
    // m_ssTableIndexQueue are done. Since both this method, and
    // tryLaunchCompaction will modify m_ssTableIndexQueue, we have to
    // guarantee that the queue is only updated by one of these at any given
    // time, otherwise, the pointer will be invalidated.
    // Also note that we can start the flushToDisk job, since it just
    // returns the SSTableIndex without modifying the ssTableIndexQueue.
    // However, we can't call checkFlushToDiskFutures while compaction job
    // is running, since checkFlushToDiskFutures will update the queue.
    log::debug("Try starting SSTable compaction job.");
    // TODO: @mli: We might be able to optimize this to take in a list of
    // filenames, so it won't need to access m_ssTableIndexQueue directly. With
    // this change, we could do checkFlushToDiskFutures while compaction is
    // running.
    auto&& ft = m_ssTableIndexQueue.tryLaunchCompaction();
    if (ft.has_value()) {
        log::debug("Starting SSTable compaction job.");
        m_ssTableCompactionFuture = move(ft.value());
    }
    log::debug("checkFlushToDiskFutures done.");
}

/**
 * NOTE: @mli:
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

ProjectDb::ProjectDb(const string& configFilePath)
    : m_impl(make_unique<ProjectDbImpl>(configFilePath)) {}
ProjectDb::~ProjectDb() = default;

optional<string> ProjectDb::get(const string& key) { return m_impl->get(key); }

void ProjectDb::set(const string& key, const string& value) {
    return m_impl->set(key, value);
}

void ProjectDb::remove(const string& key) { return m_impl->remove(key); }

}  // namespace projectdb

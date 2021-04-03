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
    optional<string> get(const string& key) {
        checkSSTableCompactionFuture();
        checkFlushToDiskFutures();

        const auto& memTableRtn = m_memTableQueue.get(key);
        if (memTableRtn.has_value()) {
            log::debug("Found key: [", key, "] in MemTableQueue with Value: [",
                       memTableRtn.value(), "]");
            return memTableRtn.value().underlyingValue();
        }

        const auto& ssTableRtn = m_ssTableIndexQueue.get(key);
        if (ssTableRtn.has_value()) {
            log::debug("Found key: [", key,
                       "] in SSTableIndexQueue with Value: [",
                       ssTableRtn.value(), "]");
            return ssTableRtn.value().underlyingValue();
        }

        log::debug("Key: [", key, "] not found.");
        return {};
    }

    void set(const string& key, const string& value) {
        checkSSTableCompactionFuture();
        checkFlushToDiskFutures();

        auto&& ft = m_memTableQueue.set(key, value);
        if (ft.has_value()) {
            log::debug("Starting flushToDisk job.");
            m_flushToDiskFutures.emplace_back(move(ft.value()));
        }
    }

    void remove(const string& key) {
        checkSSTableCompactionFuture();
        checkFlushToDiskFutures();

        auto&& ft = m_memTableQueue.remove(key);
        if (ft.has_value()) {
            log::debug("Starting flushToDisk job.");
            m_flushToDiskFutures.emplace_back(move(ft.value()));
        }
    }

   private:
    MemTableQueue m_memTableQueue;
    SSTableIndexQueue m_ssTableIndexQueue;

    // [old ... new]
    vector<future<SSTableIndex>> m_flushToDiskFutures;
    optional<future<vector<SSTableIndex>>> m_ssTableCompactionFuture;

    /**
     * Go through m_flushToDiskFutures to process the finished ones.
     * NOTE: @mli:
     * We start from the beginning (oldest one), and keep processing it and stop
     * when we reach one that is not finished. We should not try to process the
     * others after the not finished one, since this might cause
     * SSTableIndexQueue to contain out-of-order SSTableIndex.
     */
    void checkFlushToDiskFutures() {
        auto it = m_flushToDiskFutures.begin();
        while (it != m_flushToDiskFutures.end()) {
            if (it->wait_for(chrono::seconds(0)) != future_status::ready) {
                break;
            }
            auto ssTableIndex = it->get();
            log::debug("Got SSTableIndex for SSTable: ",
                       ssTableIndex.getSSTableFileName());
            auto&& ft = m_ssTableIndexQueue.insert(move(ssTableIndex));
            if (ft.has_value()) {
                if (m_ssTableCompactionFuture.has_value()) {
                    log::debug(
                        "Already an SSTable compaction job in progress. Will "
                        "not start another one.");
                } else {
                    log::debug("Starting SSTable compaction job.");
                    m_ssTableCompactionFuture = move(ft.value());
                }
            }
            // Remove the corresponding MemTable and TransactionLog.
            m_memTableQueue.pop();
            it = m_flushToDiskFutures.erase(it);
        }
    }

    /**
     * Only one compaction can be done at a single time!
     * This is because new compaction depends on the result of previous
     * compaction, since the new SSTables might be compacted into the last one
     * from previous compaction.
     */
    void checkSSTableCompactionFuture() {
        if (!m_ssTableCompactionFuture.has_value()) {
            return;
        }
        if (m_ssTableCompactionFuture.value().wait_for(chrono::seconds(0)) !=
            future_status::ready) {
            return;
        }
        auto ssTableIndexAfterCompaction =
            m_ssTableCompactionFuture.value().get();
        m_ssTableIndexQueue.update(move(ssTableIndexAfterCompaction));
        m_ssTableCompactionFuture = {};
    }
};

ProjectDb::ProjectDb() : m_impl(make_unique<ProjectDbImpl>()) {}

optional<string> ProjectDb::get(const string& key) { return m_impl->get(key); }
void ProjectDb::set(const string& key, const string& value) {
    return m_impl->set(key, value);
}
void ProjectDb::remove(const string& key) { return m_impl->remove(key); }

}  // namespace projectdb

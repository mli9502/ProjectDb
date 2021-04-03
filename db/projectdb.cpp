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
        auto&& ft = m_memTableQueue.set(key, value);
        if (ft.has_value()) {
            log::debug("Starting flushToDisk job.");
            m_flushToDiskFutures.emplace_back(move(ft.value()));
        }
    }

    void remove(const string& key) {
        auto&& ft = m_memTableQueue.remove(key);
        if (ft.has_value()) {
            log::debug("Starting flushToDisk job.");
            m_flushToDiskFutures.emplace_back(move(ft.value()));
        }
    }

   private:
    MemTableQueue m_memTableQueue;
    SSTableIndexQueue m_ssTableIndexQueue;

    vector<future<SSTableIndex>> m_flushToDiskFutures;
    vector<future<vector<SSTableIndex>>> m_ssTableCompactionFutures;

    // TODO: @mli: Add methods to handle things before/after run.
};

ProjectDb::ProjectDb() : m_impl(make_unique<ProjectDbImpl>()) {}

optional<string> ProjectDb::get(const string& key) { return m_impl->get(key); }

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 4/2/2021.
//

#ifndef MAIN_SSTABLE_INDEX_QUEUE_H
#define MAIN_SSTABLE_INDEX_QUEUE_H

#include <future>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "sstable_index.h"

using namespace std;

namespace projectdb {
class SSTableIndexQueue {
   public:
    using value_type = vector<SSTableIndex>;

    SSTableIndexQueue();

    [[nodiscard]] optional<Table::mapped_type> get(string_view key);
    [[nodiscard]] optional<future<vector<SSTableIndex>>> insert(
        SSTableIndex&& ssTableIndex);

    /**
     * Update the queue with new SSTableIndex after compaction is done.
     * @param ssTableIndexAfterCompaction
     */
    void update(vector<SSTableIndex>&& ssTableIndexAfterCompaction);

   private:
    // [old ... new]
    value_type m_queue;
    /**
     * Index to keep track of the compaction start location.
     * The SSTables that will be merged are [m_compactionStartIndex, ...,
     * m_compactionStartIndex + NUM_SSTABLE_TO_COMPACT]
     */
    size_t m_compactionStartIndex;

    /**
     * Try launching the compaction job.
     * @return
     */
    optional<future<vector<SSTableIndex>>> tryLaunchCompaction();
};
}  // namespace projectdb

#endif  // MAIN_SSTABLE_INDEX_QUEUE_H

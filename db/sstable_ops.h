//
// Created by Mengwen Li (ml4643) on 3/25/2021.
//

#ifndef MAIN_SSTABLE_OPS_H
#define MAIN_SSTABLE_OPS_H

#include <set>
#include <string>
#include <vector>

#include "sstable.h"
#include "sstable_index.h"
#include "sstable_index_queue.h"

using namespace std;

namespace projectdb {

SSTableIndex flushSSTable(const SSTable& ssTable, string_view fileName);
/**
 * loadSSTable is needed under two cases:
 * 1. When we need to merge SSTable due to compression.
 * 2. When the db is first started, or recover from crash, in this case, we need
 * ssTableIndex to be built while loading.
 * @param ssTableFileName
 * @param ssTableIndex
 * @return
 */
SSTable loadSSTable(string_view ssTableFileName,
                    SSTableIndex* ssTableIndex = nullptr);

vector<SSTableIndex> mergeSSTables(
    SSTableIndexQueue::value_type::iterator begin,
    SSTableIndexQueue::value_type::iterator end);

}  // namespace projectdb

#endif  // MAIN_SSTABLE_OPS_H

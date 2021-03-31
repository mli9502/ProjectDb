//
// Created by Mengwen Li (ml4643) on 3/25/2021.
//

#ifndef MAIN_SSTABLE_OPS_H
#define MAIN_SSTABLE_OPS_H

#include "sstable.h"
#include "sstable_index.h"

namespace projectdb {

SSTableIndex flushSSTable(const SSTable& ssTable);
/**
 * loadSSTable is needed under two cases:
 * 1. When we need to merge SSTable due to compression.
 * 2. When the db is first started, or recover from crash, in this case, we need
 * ssTableIndex to be built while loading.
 * @param ssTableFileName
 * @param ssTableIndex
 * @return
 */
SSTable loadSSTable(string_view ssTableFileName, SSTableIndex* ssTableIndex);
// TODO: @mli: Add function to merge SSTable.
}  // namespace projectdb

#endif  // MAIN_SSTABLE_OPS_H

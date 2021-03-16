//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#ifndef MAIN_SSTABLE_H
#define MAIN_SSTABLE_H

#include <memory>
#include <string_view>

#include "sstable_index.h"
#include "table.h"

using namespace std;

namespace projectdb {

/**
 * TODO: @mli: Create SSTableMergePolicy to merge SSTables. This should return a
 * new SSTable after merge.
 */

class SSTable : public Table {
   public:
    SSTable() = default;
    explicit SSTable(shared_ptr<value_type> table);

    // Flush to disk and build the index.
    [[nodiscard]] SSTableIndex flushToDisk() const;
    void loadFromDisk(string_view ssTableFileName, SSTableIndex* ssTableIndex);

   private:
    // TODO: @mli: Define a SSTableMetaData class, and implement serialization &
    // deserialization for it. The MetaData class should for now only contain a
    // timestamp.
    class SSTableMetaData {};

    SSTableMetaData m_metaData;
};

}  // namespace projectdb

#endif  // MAIN_SSTABLE_H

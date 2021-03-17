//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#ifndef MAIN_SSTABLE_H
#define MAIN_SSTABLE_H

#include <chrono>
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
    void loadFromDisk(string_view ssTableFileName,
                      SSTableIndex* ssTableIndex = nullptr);

    friend ostream& operator<<(ostream& os, const SSTable& ssTable);
    friend bool operator==(const SSTable& lhs, const SSTable& rhs);

   private:
    // TODO: @mli: Define a SSTableMetaData class, and implement serialization &
    // deserialization for it. The MetaData class should for now only contain a
    // timestamp.
    class SSTableMetaData {
       public:
        using ts_unit_type = chrono::milliseconds;

        SSTableMetaData();

        void serializeImpl(ostream& os) &&;
        SSTableMetaData deserializeImpl(istream& is) &&;

        friend ostream& operator<<(ostream& os,
                                   const SSTableMetaData& ssTableMetaData);
        friend bool operator==(const SSTableMetaData& lhs,
                               const SSTableMetaData& rhs);

       private:
        ts_unit_type::rep m_msSinceEpoch;
    };

    SSTableMetaData m_metaData;

    friend ostream& operator<<(ostream& os,
                               const SSTableMetaData& ssTableMetaData);
    friend bool operator==(const SSTableMetaData& lhs,
                           const SSTableMetaData& rhs);
};

ostream& operator<<(ostream& os, const SSTable& ssTable);
bool operator==(const SSTable& lhs, const SSTable& rhs);

ostream& operator<<(ostream& os,
                    const SSTable::SSTableMetaData& ssTableMetaData);
bool operator==(const SSTable::SSTableMetaData& lhs,
                const SSTable::SSTableMetaData& rhs);

}  // namespace projectdb

#endif  // MAIN_SSTABLE_H

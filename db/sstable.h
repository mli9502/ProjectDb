//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#ifndef MAIN_SSTABLE_H
#define MAIN_SSTABLE_H

#include <chrono>
#include <memory>
#include <string_view>

#include "system_utils.h"
#include "table.h"

using namespace std;

namespace projectdb {

/**
 * TODO: @mli: Create SSTableMergePolicy to merge SSTables. This should return a
 * new SSTable after merge.
 */

class SSTableMetaData {
   public:
    SSTableMetaData();

    void init();

    void serializeImpl(ostream& os) const&;
    SSTableMetaData deserializeImpl(istream& is) &&;

    friend ostream& operator<<(ostream& os,
                               const SSTableMetaData& ssTableMetaData);
    friend bool operator==(const SSTableMetaData& lhs,
                           const SSTableMetaData& rhs);

   private:
    timestamp_unit_type::rep m_msSinceEpoch;
};

ostream& operator<<(ostream& os, const SSTableMetaData& ssTableMetaData);
bool operator==(const SSTableMetaData& lhs, const SSTableMetaData& rhs);

class SSTable {
   public:
    /**
     * Initialize an empty SSTable.
     */
    SSTable();
    /**
     * Initialize SSTable with the given table, and SSTableMetaData will be
     * populated automatically.
     * @param table
     */
    explicit SSTable(shared_ptr<Table> table);

    SSTableMetaData& metaData();
    [[nodiscard]] const SSTableMetaData& metaData() const;

    Table& table();
    [[nodiscard]] const Table& table() const;

   private:
    SSTableMetaData m_metaData;
    shared_ptr<Table> m_table;
};

ostream& operator<<(ostream& os, const SSTable& ssTable);
bool operator==(const SSTable& lhs, const SSTable& rhs);

}  // namespace projectdb

#endif  // MAIN_SSTABLE_H

//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#ifndef MAIN_SSTABLE_INDEX_H
#define MAIN_SSTABLE_INDEX_H

#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <utility>

#include "table.h"

using namespace std;

namespace projectdb {

class SSTableIndex {
   public:
    SSTableIndex(string ssTableFileName);
    void addIndex(Table::key_type key, ios::pos_type pos);
    void setEofPos(ios::pos_type eofPos);

    optional<Table::mapped_type> seek(const Table::key_type& key) const;

    friend ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex);

   private:
    string m_ssTableFileName;
    /**
     * key: key_type
     * value: the start position of the entry with the key.
     * With the current implementation, the first and the last entry will ALWAYS
     * be in the index.
     */
    map<Table::key_type, ios::pos_type> m_index;
    ios::pos_type m_eofPos;

    optional<pair<ios::pos_type, ios::pos_type>> getPotentialBlockPos(
        const Table::key_type& key) const;
};

ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex);

}  // namespace projectdb

#endif  // MAIN_SSTABLE_INDEX_H

//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#ifndef MAIN_SSTABLE_INDEX_H
#define MAIN_SSTABLE_INDEX_H

#include <iostream>
#include <map>

#include "table.h"

using namespace std;

namespace projectdb {

class SSTableIndex {
   public:
    void addIndex(Table::key_type key, ios::pos_type pos);

    friend ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex);

   private:
    map<Table::key_type, ios::pos_type> m_index;
};

ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex);

}  // namespace projectdb

#endif  // MAIN_SSTABLE_INDEX_H

//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "sstable_index.h"

#include "log.h"

namespace projectdb {

void SSTableIndex::addIndex(Table::key_type key, ios::pos_type pos) {
    m_index[move(key)] = pos;
}

ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex) {
    os << "{ m_index: [ " << ssTableIndex.m_index << " ]}";
    return os;
}

}  // namespace projectdb

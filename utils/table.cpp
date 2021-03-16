//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#include "table.h"

namespace projectdb {

Table::~Table() = default;

shared_ptr<Table::value_type> Table::getTable() const { return m_table; }

}  // namespace projectdb

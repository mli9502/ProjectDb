//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#include "table.h"

namespace projectdb {

Table::Table() : m_table(make_unique<value_type>()) {}

const Table::value_type& Table::get() const { return *m_table; }

Table::value_type& Table::get() { return *m_table; }

}  // namespace projectdb

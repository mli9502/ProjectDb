//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#ifndef MAIN_TABLE_H
#define MAIN_TABLE_H

#include <map>
#include <memory>

#include "key.h"
#include "value.h"

using namespace std;

namespace projectdb {
/**
 * The class Table
 */
class Table {
   public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = map<key_type, mapped_type>;

    virtual ~Table() = 0;

    shared_ptr<value_type> getTable() const;

   protected:
    shared_ptr<value_type> m_table;
};

}  // namespace projectdb

#endif  // MAIN_TABLE_H

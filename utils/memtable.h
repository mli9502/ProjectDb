//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_MEMTABLE_H
#define MAIN_MEMTABLE_H

#include <map>
#include <vector>

#include "key.h"
#include "value.h"

using namespace std;

namespace projectdb {

class MemTable {
   public:
    using key_type = Key;
    using mapped_type = vector<Value>;

    // Serialization/deserialization will be used by sstable.
    void serializeImpl(ostream& os) &&;
    MemTable deserializeImpl(istream& is) &&;

    // TODO: @mli: Add get, set (When set is called, need to launch the async
    // job to convert memtable to sstable, and start serialization.)
    // TODO: @mli: Add size check? And maybe start adding callback to flush to
    // sstable?

    friend ostream& operator<<(ostream& os, const MemTable& memTable);
    friend bool operator==(const MemTable& lhs, const MemTable& rhs);

   private:
    /**
     * NOTE: @mli:
     * A vector of value is needed.
     * This is because we could have a queue of memtable.
     * In this case, when removing a key, we can't just remove the element from
     * map, we have to add a TOMBSTONE value. If we just remove the value, we
     * can't distinguish between value just never there, or it has been removed,
     * as a result, we might think the value is still there if the set and
     * delete operation are being added into different tables.
     */
    map<key_type, mapped_type> m_memTable;
};

ostream& operator<<(ostream& os, const MemTable& memTable);
bool operator==(const MemTable::mapped_type& lhs,
                const MemTable::mapped_type& rhs);
bool operator==(const MemTable& lhs, const MemTable& rhs);

}  // namespace projectdb

#endif  // MAIN_MEMTABLE_H

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "memtable.h"

#include <algorithm>

#include "serializer.h"

namespace projectdb {

void MemTable::serializeImpl(ostream& os) && {
    SerializationWrapper<map<key_type, mapped_type>>(move(m_memTable))
        .serialize(os);
}

MemTable MemTable::deserializeImpl(istream& is) && {
    m_memTable =
        SerializationWrapper<map<key_type, mapped_type>>().deserialize(is);
    return move(*this);
}

ostream& operator<<(ostream& os, const MemTable& memTable) {
    os << "{ m_memTable: [" << memTable.m_memTable << "] }";
    return os;
}

// bool operator==(const MemTable::mapped_type& lhs, const
// MemTable::mapped_type& rhs) {
//    return equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
//}

bool operator==(const MemTable& lhs, const MemTable& rhs) {
    //    return equal(lhs.m_memTable.begin(), lhs.m_memTable.end(),
    //    rhs.m_memTable.begin(), rhs.m_memTable.end());
    return lhs.m_memTable == rhs.m_memTable;
}

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "memtable.h"

#include <algorithm>
#include <optional>

#include "serializer.h"

namespace projectdb {

// https://stackoverflow.com/questions/26261007/why-is-value-taking-setter-member-functions-not-recommended-in-herb-sutters-cpp
void MemTable::set(const key_type& key, mapped_type value) {
    m_memTable[key] = move(value);
}

optional<MemTable::mapped_type> MemTable::getValueEntry(
    const key_type& key) const {
    const auto cit = m_memTable.find(key);
    if (cit == m_memTable.end()) {
        log::debug("Failed to locate key in memTable: ", key);
        return {};
    }
    return cit->second;
}

optional<MemTable::mapped_type::value_type> MemTable::getValue(
    const key_type& key) const {
    const auto entry = getValueEntry(key);
    if (!entry.has_value() || entry.value().isTombstoneValue()) {
        return {};
    }
    return entry.value().value();
}

void MemTable::remove(const key_type& key) { set(key, mapped_type{}); }

void MemTable::serializeImpl(ostream& os) && {
    SerializationWrapper<map<key_type, mapped_type>>(move(m_memTable))
        .serialize(os);
}

MemTable MemTable::deserializeImpl(istream& is) && {
    m_memTable =
        SerializationWrapper<map<key_type, mapped_type>>().deserialize(is);
    return move(*this);
}

unsigned MemTable::getApproximateSizeInBytes() const {
    unsigned rtn = 0;
    for (const auto& [k, v] : m_memTable) {
        rtn += k.getApproximateSizeInBytes() + v.getApproximateSizeInBytes();
    }
    return rtn;
}

ostream& operator<<(ostream& os, const MemTable& memTable) {
    os << "{ m_memTable: [" << memTable.m_memTable << "] }";
    return os;
}

bool operator==(const MemTable& lhs, const MemTable& rhs) {
    return lhs.m_memTable == rhs.m_memTable;
}

}  // namespace projectdb

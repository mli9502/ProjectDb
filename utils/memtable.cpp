//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "memtable.h"

#include <algorithm>
#include <memory>

#include "db_config.h"
#include "serializer.h"

namespace projectdb {

MemTable::MemTable() : Table() { m_table = make_shared<value_type>(); }

optional<MemTable::mapped_type> MemTable::getValueEntry(
    const key_type& key) const {
    const auto cit = m_table->find(key);
    if (cit == m_table->end()) {
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

// https://stackoverflow.com/questions/26261007/why-is-value-taking-setter-member-functions-not-recommended-in-herb-sutters-cpp
void MemTable::set(const key_type& key, mapped_type value) {
    (*m_table)[key] = move(value);
}

void MemTable::remove(const key_type& key) { set(key, mapped_type{}); }

bool MemTable::needsFlushToDisk() const {
    unsigned currSizeInBytes = 0;
    for (const auto& [k, v] : *m_table) {
        currSizeInBytes +=
            k.getApproximateSizeInBytes() + v.getApproximateSizeInBytes();
    }
    return currSizeInBytes >= db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;
}

ostream& operator<<(ostream& os, const MemTable& memTable) {
    os << "{ m_table: [" << *(memTable.m_table) << "] }";
    return os;
}

bool operator==(const MemTable& lhs, const MemTable& rhs) {
    return *(lhs.m_table) == *(rhs.m_table);
}

}  // namespace projectdb

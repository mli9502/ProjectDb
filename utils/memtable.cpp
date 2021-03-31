//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "memtable.h"

#include <algorithm>
#include <memory>

#include "db_config.h"
#include "serializer.h"

namespace projectdb {

MemTable::MemTable() : m_table(make_shared<Table>()) {}

/**
 * Gets the entry (with potentiall empty value) in the map where the key maps
 * to.
 */
optional<MemTable::mapped_type> MemTable::getValueEntry(
    const key_type& key) const {
    const auto& table = m_table->get();
    const auto cit = table.find(key);
    if (cit == table.end()) {
        log::debug("Failed to locate key in memTable: ", key);
        return {};
    }
    return cit->second;
}

/**
 * Gets the value of the entry (if both exist) in the map where the key maps to.
 */
optional<MemTable::mapped_type::value_type> MemTable::getValue(
    const key_type& key) const {
    const auto entry = getValueEntry(key);
    if (!entry.has_value() || entry.value().isTombstoneValue()) {
        return {};
    }
    return entry.value().value();
}

// https://stackoverflow.com/questions/26261007/why-is-value-taking-setter-member-functions-not-recommended-in-herb-sutters-cpp
/**
 * Sets a key value pair for class MemTable.
 */
void MemTable::set(const key_type& key, mapped_type value) {
    m_table->get()[key] = move(value);
}

/**
 * Clears the value for the given key.
 */
void MemTable::remove(const key_type& key) { set(key, mapped_type{}); }

/**
 * Returns true when current table size exceeds approximate max size
 * and needs to flush table to disk.
 */
bool MemTable::needsFlushToDisk() const {
    unsigned currSizeInBytes = 0;
    const auto& table = m_table->get();
    for (const auto& [k, v] : table) {
        currSizeInBytes +=
            k.getApproximateSizeInBytes() + v.getApproximateSizeInBytes();
    }
    log::debug("Current MemTable size is approximate ", currSizeInBytes,
               " bytes.");
    return currSizeInBytes >= db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;
}

shared_ptr<Table> MemTable::getTable() const { return m_table; }

ostream& operator<<(ostream& os, const MemTable& memTable) {
    os << "{ m_table: [" << memTable.m_table->get() << "] }";
    return os;
}

bool operator==(const MemTable& lhs, const MemTable& rhs) {
    return lhs.m_table->get() == rhs.m_table->get();
}

}  // namespace projectdb

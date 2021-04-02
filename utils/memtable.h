//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_MEMTABLE_H
#define MAIN_MEMTABLE_H

#include <optional>

#include "table.h"

using namespace std;

namespace projectdb {

/**
 * The class MemTable holds the representation of the database
 * in memory.
 */
class MemTable {
   public:
    using key_type = Table::key_type;
    using mapped_type = Table::mapped_type;
    using value_type = Table::value_type;

    MemTable();

    // Returns the mapped_type (Value).
    // Even if the value is deleted, return will NOT be empty because of
    // TOMBSTONE.
    [[nodiscard]] optional<mapped_type> getValueEntry(
        const key_type& key) const;
    // Returns the mapped_type::value_type (string).
    // If the value is deleted, return will be empty.
    [[nodiscard]] optional<mapped_type::value_type> getValue(
        const key_type& key) const;
    void set(const key_type& key, mapped_type value);
    /**
     * NOTE: @mli:
     * delete(key) will ALWAYS SET a TOMBSTONE value instead of actually
     * removing anything from the map. This is because a key can be presented in
     * multiple sstable, and during compression, we need TOMBSTONE to be set
     * correctly for each of these keys if delete is called during the time
     * period corresponding to the sstable. This makes sure that compression of
     * sstables works correctly.
     */
    void remove(const key_type& key);

    [[nodiscard]] bool needsFlushToDisk() const;

    [[nodiscard]] shared_ptr<Table> getTable() const;

    friend ostream& operator<<(ostream& os, const MemTable& memTable);
    friend bool operator==(const MemTable& lhs, const MemTable& rhs);

   private:
    shared_ptr<Table> m_table;
};

ostream& operator<<(ostream& os, const MemTable& memTable);
bool operator==(const MemTable& lhs, const MemTable& rhs);

}  // namespace projectdb

#endif  // MAIN_MEMTABLE_H

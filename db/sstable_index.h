//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#ifndef MAIN_SSTABLE_INDEX_H
#define MAIN_SSTABLE_INDEX_H

#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <utility>

#include "table.h"

using namespace std;

namespace projectdb {

class SSTableIndex {
   public:
    void setSSTableFileName(string_view fileName);
    string getSSTableFileName() const;
    void addIndex(Table::key_type key, ios::pos_type pos);
    void setEofPos(ios::pos_type eofPos);

    optional<Table::mapped_type> seek(const Table::key_type& key);

    friend ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex);

   private:
    /**
     * m_ssTableFileName: This is needed since when we load SSTable from disk
     * for compression, we need to know the name of the file. In this case we
     * can't just move m_ifs since during compression, it might still be used
     * for processing queries.
     */
    string m_ssTableFileName;
    /**
     * key: key_type
     * value: the start position of the entry with the key.
     * With the current implementation, the first and the last entry will ALWAYS
     * be in the index in order for us to decide if a key is in the table or not
     * quickly, and whether the last block contains the key.
     */
    map<Table::key_type, ios::pos_type> m_index;
    ios::pos_type m_eofPos;
    /**
     * m_ifs: Keeps a filestream open to prevent frequent open/close file.
     */
    unique_ptr<fstream> m_ifs;

    optional<pair<ios::pos_type, ios::pos_type>> getPotentialBlockPos(
        const Table::key_type& key) const;
};

ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex);

}  // namespace projectdb

#endif  // MAIN_SSTABLE_INDEX_H

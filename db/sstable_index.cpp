//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "sstable_index.h"

#include <filesystem>

#include "log.h"
#include "serializer.h"
#include "system_utils.h"

namespace projectdb {

void SSTableIndex::setSSTableFileName(string_view fileName) {
    m_ssTableFileName = fileName;
}

string SSTableIndex::getSSTableFileName() const { return m_ssTableFileName; }

void SSTableIndex::addIndex(Table::key_type key, ios::pos_type pos) {
    m_index[move(key)] = pos;
}

void SSTableIndex::setEofPos(ios::pos_type eofPos) { m_eofPos = eofPos; }

optional<Table::mapped_type> SSTableIndex::seek(const Table::key_type& key) {
    // NOTE: @mli: We can't initialize m_ifs in ctor, because SSTableIndex is
    // not the one that writes the data to file. It seems that if we open an
    // ifstream, then some other code opens the same file as ofstream and write
    // stuff, our already-opened ifstream will become invalid. As a result, we
    // keep a unique_ptr of ifstream, and only try to open it when seek is
    // called, since at this point, no one should be writing to the file
    // anymore.
    if (db_config::KEEP_SSTABLE_FILE_OPEN && !m_ifs) {
        m_ifs = make_unique<fstream>(
            getFileStream(m_ssTableFileName, ios_base::in));
    }

    auto potentialBlockPos = getPotentialBlockPos(key);
    if (!potentialBlockPos.has_value()) {
        return {};
    }

    Table::value_type partialSSTable;

    if (db_config::KEEP_SSTABLE_FILE_OPEN) {
        partialSSTable = DeserializationWrapper<Table::value_type>{}(
            *m_ifs, potentialBlockPos.value().first,
            potentialBlockPos.value().second);
    } else {
        auto ifs = getFileStream(m_ssTableFileName, ios_base::in);
        partialSSTable = DeserializationWrapper<Table::value_type>{}(
            ifs, potentialBlockPos.value().first,
            potentialBlockPos.value().second);
    }

    const auto cit = partialSSTable.find(key);
    if (cit == partialSSTable.end()) {
        return {};
    }
    return cit->second;
}

optional<pair<ios::pos_type, ios::pos_type>> SSTableIndex::getPotentialBlockPos(
    const Table::key_type& key) const {
    if (m_index.empty()) {
        log::error("Index is empty while trying to seek key: ", key);
        return {};
    }
    for (auto cit = m_index.cbegin(); cit != m_index.cend(); cit++) {
        if (key == cit->first) {
            ios::pos_type endPos;
            // This handles the case where the key we seek IS the last key in
            // the SSTable.
            if (cit == prev(m_index.cend())) {
                endPos = m_eofPos;
            } else {
                endPos = next(cit)->second;
            }
            log::debug("key: ", key, " is found in index at pos: ", cit->second,
                       ". Will load block: [", cit->second, ", ", endPos,
                       ") from SSTable: ", getSSTableFileName());
            return make_pair(cit->second, endPos);
        }
        if (less<Table::key_type>()(key, cit->first)) {
            if (cit == m_index.cbegin()) {
                log::debug("key: ", key,
                           " is less than the first element of the index: ",
                           cit->first, ", will skip checking SSTable: ",
                           getSSTableFileName());
                return {};
            }
            log::debug("key: ", key, " is less than index: ", cit->first,
                       " at pos: ", cit->second, ". Will load block: [",
                       prev(cit)->second, ", ", cit->second,
                       ") from SSTable: ", getSSTableFileName());
            return make_pair(prev(cit)->second, cit->second);
        }
    }
    log::debug("Key: ", key, " not found in SSTable: ", getSSTableFileName());
    return {};
}

ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex) {
    os << "{ m_index: [ " << ssTableIndex.m_index << " ], m_eofPos: ["
       << ssTableIndex.m_eofPos << "]}";
    return os;
}

}  // namespace projectdb

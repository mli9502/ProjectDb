//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "sstable_index.h"

#include <filesystem>

#include "log.h"
#include "serializer.h"
#include "system_utils.h"

namespace projectdb {

SSTableIndex::SSTableIndex(string ssTableFileName)
    : m_ssTableFileName(move(ssTableFileName)) {}

void SSTableIndex::addIndex(Table::key_type key, ios::pos_type pos) {
    m_index[move(key)] = pos;
}

void SSTableIndex::setEofPos(ios::pos_type eofPos) { m_eofPos = eofPos; }

optional<Table::mapped_type> SSTableIndex::seek(
    const Table::key_type& key) const {
    auto potentialBlockPos = getPotentialBlockPos(key);
    if (!potentialBlockPos.has_value()) {
        return {};
    }

    // Load the block from file.
    auto fs = getFileStream(m_ssTableFileName, ios::in);

    Table::value_type partialSSTable =
        DeserializationWrapper<Table::value_type>{}(
            fs, potentialBlockPos.value().first,
            potentialBlockPos.value().second);

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
                       ". Will load block: [", cit->second, ", ", endPos, ").");
            return make_pair(cit->second, endPos);
        }
        if (less<Table::key_type>()(key, cit->first)) {
            if (cit == m_index.cbegin()) {
                log::debug("key: ", key,
                           " is less than the first element of the index: ",
                           cit->first);
                return {};
            }
            log::debug("key: ", key, " is less than index: ", cit->first,
                       " at pos: ", cit->second, ". Will load block: [",
                       prev(cit)->second, ", ", cit->second, ").");
            return make_pair(prev(cit)->second, cit->second);
        }
    }
    log::debug("Key: ", key, " not found in SSTable: ", m_ssTableFileName);
    return {};
}

ostream& operator<<(ostream& os, const SSTableIndex& ssTableIndex) {
    os << "{ m_index: [ " << ssTableIndex.m_index << " ], m_eofPos: ["
       << ssTableIndex.m_eofPos << "]}";
    return os;
}

}  // namespace projectdb

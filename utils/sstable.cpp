//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#include "sstable.h"

#include <chrono>
#include <filesystem>
#include <fstream>
// TODO: @mli: remove thread when we remove sleep.
#include <thread>

#include "db_config.h"
#include "log.h"
#include "serializer.h"
#include "system_utils.h"

namespace projectdb {

namespace {

string genSSTableFileName() {
    static unsigned ssTableFileCounter = 0;
    return db_config::DB_FILE_PREFIX + "_" + to_string(ssTableFileCounter++) +
           "_" + to_string(getProcessId()) + "." + db_config::SSTABLE_FILE_TYPE;
}

}  // namespace

SSTable::SSTable(shared_ptr<value_type> table) : Table(), m_metaData() {
    m_table = move(table);
}

SSTableIndex SSTable::flushToDisk() const {
    SSTableIndex rtn;
    auto fs = getFileStream(genSSTableFileName(), ios::out);
    SerializationWrapper<decltype(m_metaData)>(m_metaData).serialize(fs);
    SerializationWrapper<value_type>(*m_table).serialize(
        fs, [&](value_type::value_type& entry, ios::pos_type pos,
                streamsize currBlockSize, bool isFirstOrLastEntry) {
            if (!isFirstOrLastEntry &&
                currBlockSize < db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES) {
                return false;
            }
            rtn.addIndex(entry.first, pos);
            return true;
        });
    // TODO: @mli: Remove this sleep.
    this_thread::sleep_for(chrono::seconds(5));
    return rtn;
}

/**
 * Load and deserialize SSTable from disk with given file name.
 * Note that we don't always need index to be built.
 * Index needs to be built when we load from disk to recover from failure.
 * Index doesn't need to be built if we load from disk to merge SSTables. In
 * this case, ssTableIndex will be nullptr.
 * @param ssTableFileName
 * @param ssTableIndex
 */
void SSTable::loadFromDisk(string_view ssTableFileName,
                           SSTableIndex* ssTableIndex) {
    // TODO: @mli: Add code to read from file, deserialize, and populate
    // m_table. If ssTableIndex is not null, also populate the index. This is
    // needed when we load SSTable back from disk to recover from a crash.
    auto fs = getFileStream(ssTableFileName, ios::in);
    m_metaData = SerializationWrapper<decltype(m_metaData)>().deserialize(fs);
    m_table =
        make_shared<value_type>(SerializationWrapper<value_type>().deserialize(
            fs, [&](value_type::value_type& entry, ios::pos_type pos,
                    streamsize currBlockSize, bool isFirstOrLastEntry) {
                if (!ssTableIndex) {
                    return false;
                }
                if (!isFirstOrLastEntry &&
                    currBlockSize <
                        db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES) {
                    return false;
                }
                ssTableIndex->addIndex(entry.first, pos);
                return true;
            }));
    log::debug("Successfully deserialzed SSTable: ", *this);
}

ostream& operator<<(ostream& os, const SSTable& ssTable) {
    os << "{ m_metaData: [" << ssTable.m_metaData << "], m_table: [ "
       << *(ssTable.m_table) << " ]}";
    return os;
}

SSTable::SSTableMetaData::SSTableMetaData()
    : m_msSinceEpoch(chrono::duration_cast<ts_unit_type>(
                         chrono::system_clock::now().time_since_epoch())
                         .count()) {}

void SSTable::SSTableMetaData::serializeImpl(ostream& os) && {
    SerializationWrapper<ts_unit_type::rep>(m_msSinceEpoch).serialize(os);
}

SSTable::SSTableMetaData SSTable::SSTableMetaData::deserializeImpl(
    istream& is) && {
    m_msSinceEpoch = SerializationWrapper<ts_unit_type::rep>().deserialize(is);
    return move(*this);
}

ostream& operator<<(ostream& os,
                    const SSTable::SSTableMetaData& ssTableMetaData) {
    os << "{ m_msSinceEpoch: [" << ssTableMetaData.m_msSinceEpoch << "] }";
    return os;
}
bool operator==(const SSTable::SSTableMetaData& lhs,
                const SSTable::SSTableMetaData& rhs) {
    return lhs.m_msSinceEpoch == rhs.m_msSinceEpoch;
}

}  // namespace projectdb

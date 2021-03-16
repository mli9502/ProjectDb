//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#include "sstable.h"

// TODO: @mli: remove chrono & thread when we remove sleep.
#include <chrono>
#include <thread>

#include "log.h"

namespace projectdb {

namespace {

string genSSTableFileName() {
    // TODO: @mli: Add code to generate file name.
    return "";
}

}  // namespace

SSTable::SSTable(shared_ptr<value_type> table) : Table() {
    m_table = move(table);
}

SSTableIndex SSTable::flushToDisk() const {
    const auto ssTableFileName = genSSTableFileName();
    log::debug("Flush SSTable to disk with file name: ", ssTableFileName);
    // TODO: @mli: Add code to open file, serialize, and flush to disk. Remember
    // to include the timestamp as metadata.
    this_thread::sleep_for(chrono::seconds(5));
    return SSTableIndex();
}

void SSTable::loadFromDisk(string_view ssTableFileName,
                           SSTableIndex* ssTableIndex) {}

void loadFromDisk(string_view ssTableFileName, SSTableIndex* ssTableIndex) {
    // TODO: @mli: Add code to read from file, deserialize, and populate
    // m_table. If ssTableIndex is not null, also populate the index. This is
    // needed when we load SSTable back from disk to recover from a crash.
    return;
}

// void MemTable::serializeImpl(ostream& os) && {
//    SerializationWrapper<map<key_type, mapped_type>>(move(m_table))
//        .serialize(os);
//}
//
// MemTable MemTable::deserializeImpl(istream& is) && {
//    m_table =
//        SerializationWrapper<map<key_type, mapped_type>>().deserialize(is);
//    return move(*this);
//}

}  // namespace projectdb

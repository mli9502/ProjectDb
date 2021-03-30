//
// Created by Mengwen Li (ml4643) on 3/15/2021.
//

#include "sstable.h"

#include <chrono>
#include <filesystem>
#include <fstream>

#include "db_config.h"
#include "log.h"
#include "serializer.h"
#include "system_utils.h"

namespace projectdb {

SSTableMetaData::SSTableMetaData() : m_msSinceEpoch(0) {}

void SSTableMetaData::init() { m_msSinceEpoch = getTimeSinceEpoch().count(); }

void SSTableMetaData::serializeImpl(ostream& os) const& {
    SerializationWrapper<timestamp_unit_type::rep>{m_msSinceEpoch}(os);
}

SSTableMetaData SSTableMetaData::deserializeImpl(istream& is) && {
    m_msSinceEpoch = DeserializationWrapper<timestamp_unit_type::rep>{}(is);
    return move(*this);
}

ostream& operator<<(ostream& os, const SSTableMetaData& ssTableMetaData) {
    os << "{ m_msSinceEpoch: [" << ssTableMetaData.m_msSinceEpoch << "] }";
    return os;
}
bool operator==(const SSTableMetaData& lhs, const SSTableMetaData& rhs) {
    return lhs.m_msSinceEpoch == rhs.m_msSinceEpoch;
}

SSTable::SSTable() : m_metaData(), m_table(make_shared<Table>()) {}

SSTable::SSTable(shared_ptr<Table> table) : m_metaData(), m_table(move(table)) {
    m_metaData.init();
}

SSTableMetaData& SSTable::metaData() { return m_metaData; }
const SSTableMetaData& SSTable::metaData() const { return m_metaData; }

Table& SSTable::table() { return *m_table; }
const Table& SSTable::table() const { return *m_table; }

ostream& operator<<(ostream& os, const SSTable& ssTable) {
    os << "{ m_metaData: [" << ssTable.metaData() << "], m_table: [ "
       << ssTable.table().get() << " ]}";
    return os;
}
bool operator==(const SSTable& lhs, const SSTable& rhs) {
    return (lhs.metaData() == rhs.metaData()) &&
           (lhs.table().get() == rhs.table().get());
}

}  // namespace projectdb

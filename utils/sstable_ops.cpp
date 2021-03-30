//
// Created by Mengwen Li (ml4643) on 3/25/2021.
//

#include "sstable_ops.h"

#include "db_config.h"
#include "serializer.h"

namespace projectdb {
SSTableIndex flushSSTable(const SSTable& ssTable) {
    const auto fileName = genSSTableFileName();

    SSTableIndex rtn(fileName);

    auto ofs = getFileStream(fileName, ios_base::out);
    SerializationWrapper<SSTableMetaData>{ssTable.metaData()}(ofs);
    SerializationWrapper<Table::value_type>{ssTable.table().get()}(
        ofs, [&](const Table::value_type::value_type& entry, ios::pos_type pos,
                 streamsize currBlockSize, bool isFirstOrLastEntry) {
            if (!isFirstOrLastEntry &&
                currBlockSize < db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES) {
                return false;
            }
            rtn.addIndex(entry.first, pos);
            return true;
        });
    rtn.setEofPos(ofs.tellp());
    return rtn;
}

SSTable loadSSTable(string_view ssTableFileName, SSTableIndex* ssTableIndex) {
    auto ifs = getFileStream(ssTableFileName, ios::in);

    SSTable rtn;

    rtn.metaData() = DeserializationWrapper<SSTableMetaData>{}(ifs);
    rtn.table().get() = DeserializationWrapper<Table::value_type>{}(
        ifs, [&](const Table::value_type::value_type& entry, ios::pos_type pos,
                 streamsize currBlockSize, bool isFirstOrLastEntry) {
            if (!ssTableIndex) {
                return false;
            }
            if (!isFirstOrLastEntry &&
                currBlockSize < db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES) {
                return false;
            }
            ssTableIndex->addIndex(entry.first, pos);
            return true;
        });
    if (ssTableIndex) {
        ssTableIndex->setEofPos(ifs.tellg());
    }

    log::debug("Successfully deserialzed SSTable: ", rtn);
    return rtn;
}
}  // namespace projectdb

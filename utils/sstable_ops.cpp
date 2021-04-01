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

SSTableMerger::SSTableMerger(set<string> ssTableFileNames)
    : m_ssTableFileNames(move(ssTableFileNames)) {}

vector<SSTableIndex> SSTableMerger::operator()() && {
    vector<SSTableIndex> rtn;
    if (m_ssTableFileNames.empty()) {
        return rtn;
    }
    // NOTE: @mli: The merged SSTable will have the same file name as the FIRST
    // SSTable that it gets.
    // TODO: @mli:
    // 0. Update SSTableIndex to have a setFileName method, instead of taking
    // filename in ctor, since it's a little unclear if it takes the filename in
    // ctor, when will the ifs be initialized.
    // 1. Probably need to rename this to SSTableCompactionManager, since it
    // need to track things like the index in SSTableQueue we should start
    // compaction from.
    //      Everytime we finish compaction, we should update this index to point
    //      to the last SSTable in the list of tables that are compacted,
    //      because we might still be able to compact things into it. Also it
    //      needs to track the number of SSTableIndex in the queue after each
    //      compaction is finished, so that it knows when a new compaction
    //      should start.
    // 2. We should have a SSTABLE_NUM_FILES_TO_MERGE to represent the number of
    // new SSTables we should wait to be added before we perform compaction.
    //      Probably set it to 5 for now? Which means that we start the
    //      compaction everytime 5 new tables are added.
    // 3. We also need to have a SSTABLE_MAX_SIZE to represent the max size an
    // SSTable could be before we stop merging to it.
    // 4. In the merge async job, we need to do the following things:
    //      a. Remove all the files on disk that are marked as .DEPRECATED.
    //      These markers are set when we handle a finished merge async job
    //      during set/get/remove.
    //          Note that when handling finished merge async job, we can't
    //          remove file at that time, since the remove operation might be
    //          expensive.
    //      b. merge SSTable, generate SSTableIndex WITH NORMAL FILENAME, then,
    //      write the merged SSTable to .MERGED file.
    //          Note that during the async job, we can't modify the existing
    //          SSTable files, since they are still needed to process queries.
    // 5. When processing finished compaction async job, we need to do the
    // following things:
    //      a. Remove all the SSTableIndex that has been affected from
    //      SSTableQueue. b. Rename the files associated with the removed
    //      SSTableIndex to .DEPRECATED. c. Put the merged SSTableIndex into the
    //      queue. d. Rename the files associated with the merged SSTableIndex
    //      to normal (Remove the .MERGED).
}

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 3/25/2021.
//

#include "sstable_ops.h"

#include "db_config.h"
#include "serializer.h"
#include "system_utils.h"

namespace projectdb {

namespace {

unique_ptr<SSTable> mergeSSTable(const SSTable& oldSSTable,
                                 const SSTable& newSSTable) {
    const Table::value_type& oldTable = oldSSTable.table().get();
    const Table::value_type& newTable = newSSTable.table().get();

    unique_ptr<SSTable> rtn = make_unique<SSTable>();
    Table::value_type& mergedTable = rtn->table().get();

    auto oldIt = oldTable.begin();
    auto newIt = newTable.begin();

    while (oldIt != oldTable.end() && newIt != newTable.end()) {
        if (oldIt == oldTable.end()) {
            mergedTable.emplace(*oldIt);
            oldIt++;
        } else if (newIt == newTable.end()) {
            mergedTable.emplace(*newIt);
            newIt++;
        } else if (oldIt->first < newIt->first) {
            mergedTable.emplace(*oldIt);
            oldIt++;
        } else if (newIt->first < oldIt->first) {
            mergedTable.emplace(*newIt);
            newIt++;
        } else {
            mergedTable.emplace(*newIt);
            oldIt++;
            newIt++;
        }
    }

    // Go through merged table and remove all entries with TOMBSTONE value.
    for (auto it = mergedTable.cbegin(); it != mergedTable.cend();) {
        if (it->second.isTombstoneValue()) {
            it = mergedTable.erase(it);
        } else {
            it++;
        }
    }

    log::debug("oldTable: ", oldTable);
    log::debug("newTable: ", newTable);
    log::debug("mergedTable: ", mergedTable);

    return rtn;
}

}  // namespace

SSTableIndex flushSSTable(const SSTable& ssTable, string_view fileName) {
    SSTableIndex rtn;

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
    rtn.setSSTableFileName(fileName);

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

vector<SSTableIndex> mergeSSTables(
    SSTableIndexQueue::value_type::iterator begin,
    SSTableIndexQueue::value_type::iterator end) {
    vector<SSTableIndex> rtn;
    auto curr = begin;
    auto next = ++begin;
    if (next == end) {
        log::errorAndThrow(
            "Only one element to merge! This should not happen when "
            "NUM_SSTABLE_TO_COMPACT > 0!");
    }
    auto currSSTableFileName = curr->getSSTableFileName();
    auto currMergedTableFileName =
        genMergedSSTableFileName(currSSTableFileName);
    unique_ptr<SSTable> currTable =
        make_unique<SSTable>(loadSSTable(currSSTableFileName));
    auto currTableSize = getFileSizeInBytes(currSSTableFileName);
    while (true) {
        auto nextTable =
            make_unique<SSTable>(loadSSTable(next->getSSTableFileName()));
        currTableSize += getFileSizeInBytes(next->getSSTableFileName());
        // Merge to a new table.
        currTable = mergeSSTable(*currTable, *nextTable);
        next++;
        log::debug("currTable after merge: ", *currTable);
        log::debug("currTableSize after merge: ", currTableSize);
        if (next == end) {
            log::debug("next reaches end, flush to disk.");
            rtn.emplace_back(flushSSTable(
                *currTable, genMergedSSTableFileName(currSSTableFileName)));
            break;
        }
        if (currTableSize > db_config::SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES) {
            log::debug(
                "currTableSize > SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES, flush "
                "to disk.");
            rtn.emplace_back(flushSSTable(
                *currTable, genMergedSSTableFileName(currSSTableFileName)));
            curr = next;
            next++;
            currSSTableFileName = curr->getSSTableFileName();
            currMergedTableFileName =
                genMergedSSTableFileName(currSSTableFileName);
            currTable = make_unique<SSTable>(loadSSTable(currSSTableFileName));
            currTableSize = getFileSizeInBytes(currSSTableFileName);

            if (next == end) {
                rtn.emplace_back(flushSSTable(
                    *currTable, genMergedSSTableFileName(currSSTableFileName)));
                break;
            }
        }
    }
    return rtn;
}

// NOTE: @mli: The merged SSTable will have the same file name as the FIRST
// SSTable that it gets.
// TODO: @mli:
// [x] 0. Update SSTableIndex to have a setFileName method, instead of taking
// filename in ctor, since it's a little unclear if it takes the filename in
// ctor, when will the ifs be initialized.
// [ ] 1. Probably need to rename this to SSTableCompactionManager, since it
// need to track things like the index in SSTableQueue we should start
// compaction from.
//      Everytime we finish compaction, we should update this index to point
//      to the last SSTable in the list of tables that are compacted,
//      because we might still be able to compact things into it. Also it
//      needs to track the number of SSTableIndex in the queue after each
//      compaction is finished, so that it knows when a new compaction
//      should start.
// [ ] 2. We should have a NUM_SSTABLE_TO_COMPACT to represent the number of
// new SSTables we should wait to be added before we perform compaction.
//      Probably set it to 5 for now? Which means that we start the
//      compaction everytime 5 new tables are added.
// [ ] 3. We also need to have a SSTABLE_MAX_SIZE_IN_BYTES to represent the max
// size an SSTable could be before we stop merging to it. [ ] 4. In the merge
// async job, we need to do the following things:
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
// [ ] 5. When processing finished compaction async job, we need to do the
// following things:
//      a. Remove all the SSTableIndex that has been affected from
//      SSTableQueue. b. Rename the files associated with the removed
//      SSTableIndex to .DEPRECATED. c. Put the merged SSTableIndex into the
//      queue. d. Rename the files associated with the merged SSTableIndex
//      to normal (Remove the .MERGED).

}  // namespace projectdb

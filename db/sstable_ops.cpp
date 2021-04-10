//
// Created by Mengwen Li (ml4643) on 3/25/2021.
//

#include "sstable_ops.h"

#include "db_config.h"
#include "serializer.h"
#include "system_utils.h"

namespace projectdb {

namespace {

/**
 * NOTE: @mli:
 * It is important that when we do compaction, we can't blindly remove all
 * entries with TOMBSTONE value. Consider this situation, we have 4 SSTables,
 * and the size limitation requires us to compact the first 2, and then the
 * second 2. If a key contains a value in table 1, and is marked as TOMBSTONE in
 * table 3, the, if we remove all entries that's TOMBSTONE during compaction,
 * merged table 1 will still has this value (due to 3 not merged with 1), but
 * merged table 3 will not have a TOMBSTONE entry for this key. This will cause
 * that even the key is already deleted, it will re-appear when get.
 *
 * To fix this, during compaction, we can only remove the entry from the merged
 * table if we see the value first, then see a TOMBSTONE. For TOMBSTONE that
 * does not correspond to a value, we have to still keep them in the merged
 * table.
 *
 * @param oldSSTable
 * @param newSSTable
 * @return
 */
unique_ptr<SSTable> mergeSSTable(const SSTable& oldSSTable,
                                 const SSTable& newSSTable) {
    const Table::value_type& oldTable = oldSSTable.table().get();
    const Table::value_type& newTable = newSSTable.table().get();

    unique_ptr<SSTable> rtn = make_unique<SSTable>();
    Table::value_type& mergedTable = rtn->table().get();

    auto oldIt = oldTable.begin();
    auto newIt = newTable.begin();

    while (oldIt != oldTable.end() && newIt != newTable.end()) {
        if (oldIt->first < newIt->first) {
            mergedTable.emplace(*oldIt);
            oldIt++;
        } else if (newIt->first < oldIt->first) {
            mergedTable.emplace(*newIt);
            newIt++;
        } else {
            // When key exists in both newTable and oldTable, we only add it to
            // mergedTable if it's not TOMBSTONE.
            if (!newIt->second.isTombstoneValue()) {
                mergedTable.emplace(*newIt);
            }
            oldIt++;
            newIt++;
        }
    }

    while (oldIt != oldTable.end()) {
        mergedTable.emplace(*oldIt);
        oldIt++;
    }

    while (newIt != newTable.end()) {
        mergedTable.emplace(*newIt);
        newIt++;
    }

    log::debug("oldTable: ", oldTable);
    log::debug("newTable: ", newTable);
    log::debug("mergedTable: ", mergedTable);
    return rtn;
}

}  // namespace

SSTableIndex flushSSTable(const SSTable& ssTable, string_view fileName) {
    // First try remove deprecated files to keep directory clean.
    removeFilesWithExt(db_config::impl::DEPRECATED_FILE_EXT);

    SSTableIndex rtn;
    // Scope to make sure that ofs is destroyed.
    {
        auto ofs = getFileStream(fileName, ios_base::out);
        SerializationWrapper<SSTableMetaData>{ssTable.metaData()}(ofs);
        SerializationWrapper<Table::value_type>{ssTable.table().get()}(
            ofs,
            [&](const Table::value_type::value_type& entry, ios::pos_type pos,
                streamsize currBlockSize, bool isFirstOrLastEntry) {
                if (!isFirstOrLastEntry &&
                    currBlockSize <
                        db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES) {
                    return false;
                }
                rtn.addIndex(entry.first, pos);
                return true;
            });
        rtn.setEofPos(ofs.tellp());
        // Need to flush here to make sure that file is there.
        ofs.flush();
    }

    rtn.setSSTableFileName(fileName);
    // Wait to make sure that the file is actually there.
    waitUntilFileExist(fileName);
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
        ssTableIndex->setSSTableFileName(ssTableFileName);
    }

    log::debug("Successfully deserialzed SSTable: ", rtn);
    return rtn;
}

vector<SSTableIndex> mergeSSTables(
    SSTableIndexQueue::value_type::iterator begin,
    SSTableIndexQueue::value_type::iterator end) {
    // First try remove deprecated files to keep directory clean.
    removeFilesWithExt(db_config::impl::DEPRECATED_FILE_EXT);

    auto debugTag = to_string(getTimeSinceEpoch().count()) + "; ";

    vector<SSTableIndex> rtn;
    auto curr = begin;
    auto next = ++begin;
    if (next == end) {
        log::errorAndThrow(
            "Only one element to merge! This should not happen when "
            "NUM_SSTABLE_TO_COMPACT > 0!");
    }
    auto currSSTableFileName = curr->getSSTableFileName();
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
            currTable = make_unique<SSTable>(loadSSTable(currSSTableFileName));
            currTableSize = getFileSizeInBytes(currSSTableFileName);

            if (next == end) {
                log::debug(
                    "Only one SSTable left. Will just mark it as merged.");
                rtn.emplace_back(flushSSTable(
                    *currTable, genMergedSSTableFileName(currSSTableFileName)));
                break;
            }
        }
    }
    return rtn;
}

}  // namespace projectdb

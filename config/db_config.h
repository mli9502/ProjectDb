//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#ifndef MAIN_DB_CONFIG_H
#define MAIN_DB_CONFIG_H

#include <string>

using namespace std;

namespace projectdb {

namespace db_config {

/**
 * TODO: @mli: Add a config file parser to set these variables when db starts.
 * json parser maybe?
 */
extern unsigned MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;
extern unsigned SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES;

// Prefix shared by all files generated by database.
extern string DB_FILE_PREFIX;
// Path that database should read/write file from/to.
extern string DB_FILE_PATH;

// The file extensions.
extern string SSTABLE_FILE_EXT;
extern string SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT;
extern string MERGED_SSTABLE_FILE_EXT;

extern string TRANSACTION_LOG_FILE_EXT;

extern string DEPRECATED_FILE_EXT;

// The number of new SSTables that needs to be added before running a
// compaction. Compaction runs on these newly added SSTables.
extern unsigned NUM_SSTABLE_TO_COMPACT;
// The max size in bytes for a SSTable after compaction.
// IMPORTANT: This size SHOULD NOT be smaller than
// MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES. During compaction, at least one merge
// will be done before checking this size. So it is possible that the actual
// size of SSTable on disk is larger than this.
extern unsigned SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;

// This controls whether SSTableIndex internally holds an ifstream of the
// corresponding SSTable file, and keeps SSTable file open. When set to true,
// this will make 'get' operation performs better. However, this could lead to
// too many file handles open exception depending on OS that the db runs on.
// This is by default disabled.
extern bool KEEP_SSTABLE_FILE_OPEN;

// Variables inside impl namespace are parameters that are updated
// __internally__ by database. These vars SHOULD NOT be exposed for users to
// modify.
namespace impl {
// The counter that we should start with when writing a new file to disk.
// These are needed because when recovering from failure or application restart,
// we need to continue writing files, instead of starting from 0,
// which will overwrite the old files by mistake.
extern unsigned SSTABLE_FILE_COUNTER_BASE;
extern unsigned TRANSACTION_LOG_FILE_COUNTER_BASE;
}  // namespace impl

}  // namespace db_config

}  // namespace projectdb

#endif  // MAIN_DB_CONFIG_H

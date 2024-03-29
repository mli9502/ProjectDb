//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "db_config.h"

namespace projectdb {

namespace db_config {

// Default to write to current directory.
string DB_FILE_PATH = "./projectdb";

unsigned MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 2 * 1024 * 1024;  // 2 mb

unsigned SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 4 * 1024 * 1024;  // 4 mb

unsigned SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 2 * 1024;  // 2 kb

unsigned NUM_SSTABLE_TO_COMPACT = 5;

bool KEEP_SSTABLE_FILE_OPEN = false;

namespace impl {

unsigned SSTABLE_FILE_COUNTER_BASE = 0;
unsigned TRANSACTION_LOG_FILE_COUNTER_BASE = 0;

string DB_FILE_PREFIX = "project_db";

string SSTABLE_FILE_EXT = ".sst";
string SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT = ".ip";
string MERGED_SSTABLE_FILE_EXT = ".merged";

string TRANSACTION_LOG_FILE_EXT = ".txl";

string DEPRECATED_FILE_EXT = ".deprecated";

}  // namespace impl

}  // namespace db_config

}  // namespace projectdb

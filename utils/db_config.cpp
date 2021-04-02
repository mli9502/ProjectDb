//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "db_config.h"

namespace projectdb {

namespace db_config {

unsigned MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 16 * 1024 * 1024;  // 16 mb
unsigned SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 512 * 1024;             // 0.5 mb

string DB_FILE_PREFIX = "project_db";

// Default to write to current directory.
string DB_FILE_PATH = "./projectdb";

string SSTABLE_FILE_TYPE = "sst";
string TRANSACTION_LOG_FILE_TYPE = "txl";
string MERGED_SSTABLE_FILE_TYPE = "merged";
string DEPRECATED_FILE_TYPE = "deprecated";

unsigned NUM_SSTABLE_TO_COMPACT = 5;
unsigned SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 64 * 1024 * 1024;  // 64 mb

namespace impl {
unsigned SSTABLE_FILE_COUNTER_BASE = 0;
unsigned TRANSACTION_LOG_FILE_COUNTER_BASE = 0;
}  // namespace impl

}  // namespace db_config

}  // namespace projectdb

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

}  // namespace db_config

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 4/10/2021.
//

#include "test_utils.h"

#include <filesystem>

#include "db_config.h"

using namespace std;

namespace projectdb {
namespace test {

// Applies an unrealistic config just for testing.
// This will result in a lot of flushes to disk.
void apply_testing_db_config() {
    db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 10;
    db_config::SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 20;
    db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 8;
    db_config::NUM_SSTABLE_TO_COMPACT = 2;
}

void try_remove_db_dir() {
    filesystem::path dir("./projectdb");
    if (!filesystem::exists(dir)) {
        return;
    }
    filesystem::remove_all(dir);
}

}  // namespace test
}  // namespace projectdb

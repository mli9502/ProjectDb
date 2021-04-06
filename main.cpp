#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "db_config.h"
#include "key.h"
#include "log.h"
#include "memtable_queue.h"
#include "serializer.h"
#include "sstable.h"
#include "sstable_index.h"
#include "table.h"
#include "transaction_log.h"
#include "value.h"

#include "projectdb/projectdb.h"
#include "bench.h"

using namespace std;
using namespace projectdb;

const int data_size = 10'000;
const int word_len = 10;

int main() {
	db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 10;
    db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 8;
    db_config::NUM_SSTABLE_TO_COMPACT = 2;
    db_config::SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 20;

    ProjectDb db;
//	struct bench_stats bs = run_bench(db, data_size, word_len);
//	print_stats(bs);
}

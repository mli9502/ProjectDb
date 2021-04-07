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

const int data_size = 1;
const int word_len = 100;

int main() {
	struct bench_stats bs;
	run_bench(bs, data_size, word_len);
	print_stats(bs);
}

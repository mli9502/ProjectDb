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

using kvp_vec = vector<pair<string,string>>;

const int data_size = 30000;
const int word_len = 100;

int main() {
    cout << "Starting..." << endl;
	struct bench_stats bs;
	kvp_vec kvs = gen_rand(data_size, word_len);
	run_bench(bs, kvs);
	print_stats(bs);

//	kvs = read_csv("../benchmarks/USvideos.csv",data_size);
//	if(data_size != kvs.size()) {
//	    cerr << "Error parsing file..." << endl;
//	    return -1;
//	}
//	run_bench(bs,kvs);
//	print_stats(bs);
}

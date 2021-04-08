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

const int data_size = 10'000;
const int word_len = 100;

int main() {
	struct bench_stats bs;
	kvp_vec kvs = gen_rand(data_size, word_len);
	
	auto start = chrono::steady_clock::now();
	run_bench(bs, kvs);
	auto stop = chrono::steady_clock::now();
	cout<<chrono::duration<double>(stop-start).count()<<"\n";
	cout<<"Randomly generated:\n";
	print_stats(bs);

	kvs = read_csv("../datasets/USvideos.csv",2,data_size);
	sort(kvs.begin(),kvs.end());
	run_bench(bs,kvs);
	cout<<"Youtube data set:\n";
	print_stats(bs);
}

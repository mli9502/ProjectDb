//
// Created by Mengwen Li (ml4643) on 4/8/2021.
//

#include <chrono>
#include <iostream>

#include "bench.h"
#include "projectdb/projectdb.h"

using namespace std;
using namespace projectdb;

using kvp_vec = vector<pair<string, string>>;

const int word_len = 100;

int main() {
	for (int data_size = 24'000; data_size<50'001; data_size*=2) {
		cout << "Starting..." << endl;
		struct bench_stats bs;
		kvp_vec kvs = gen_rand(data_size, word_len);

		auto start = chrono::steady_clock::now();
		run_bench(bs, kvs);
		auto stop = chrono::steady_clock::now();
		cout << chrono::duration<double>(stop - start).count() << "\n";
		cout << "Randomly generated:\n";
		cout<<kvs.size()<<" entries\n";
		print_stats(bs);

		kvs = read_csv("../../benchmark/USvideos.csv",2,data_size);
		sort(kvs.begin(),kvs.end());
		run_bench(bs,kvs);
		cout<< "Using dataset USvideos.csv\n";
		cout<<kvs.size()<<" entries\n";
		print_stats(bs);
	}
}

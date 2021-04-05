#ifndef BENCHMARKS
#define BENCHMARKS

#include <chrono>
#include <iostream>
#include <vector>

#include "projectdb/projectdb.h"
#include "random.h"

using namespace std;
using namespace projectdb;

using kvp_vec = vector<pair<string,string>>;

// Benchmarks modifed from LevelDB
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Benchmarks avaliable:
//       fillseq       -- write N values in sequential key order
//       fillrandom    -- write N values in random key order
//       overwrite     -- overwrite N values in random key order
//       deleteseq     -- delete N keys in sequential order
//       deleterandom  -- delete N keys in random order
//       seekrandom    -- N random seeks
//       seekordered   -- N ordered seeks

/**
 * Contains duration statistics for all benchmarks.
 */
struct bench_stats {
	chrono::duration<double> fillseq;
	chrono::duration<double> fillrandom;
	chrono::duration<double> overwrite;
	chrono::duration<double> deleteseq;
	chrono::duration<double> deleterandom;
	chrono::duration<double> seekrandom;
	chrono::duration<double> seekordered;
};

// Useful functions for managing csv
void print_db(ProjectDb& db, const kvp_vec& kvs);
kvp_vec read_csv(const string fname, int val_col, int size);
void csv_db (const string fname, ProjectDb& db, int val_col, int size);

// Benchmark test
struct bench_stats run_bench(ProjectDb& db, int size, int len);

#endif

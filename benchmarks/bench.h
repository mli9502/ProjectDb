#include <chrono>

using namespace chrono

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
	duration<double> fillseq;
	duration<double> fillrandom;
	duration<double> overwrite;
	duration<double> deleteseq;
	duration<double> deleterandom;
	duration<double> seekrandom;
	duration<double> seekordered;
}

void print_db(ProjectDb db, vector<string> kvs)
vector<pair<string,string>> read_csv(string fname, int val_col, int size)
vector<pair<string,string>> copy_sort(const vector<pair<string,string>> kvs)
string random_str(const Random &rnd, int len)
string random_key(const Random &rnd, int len)
vector<pair<string,string>> gen_rand(int seed, int size, int len)

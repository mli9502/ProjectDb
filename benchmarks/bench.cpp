//
// Created by smao on 3/29/21.
//
#include <iostream>
#include "project_db.h"
#include "random.h"
#include "bench.h"

using namespace std;
using namespace projectdb;

using kvp_vec = vector<pair<string,string>>;

/**
 * Prints the keys and values in db
 * given a vector of key and value pairs.
 */
void print_db(ProjectDb& db, const kvp_vec& kvs) 
{
	for (auto pair : kvs)
		cout<<pair.first<<", "<<pair.second<<'\n';
}

/**
 * Reads the first size number of rows from the given csv file 
 * into a vector of key value pairs where the keys are taken from the 
 * first column and the values are taken from val_col.
 */
kvp_vec read_csv(const string fname, int val_col, int size)
{
	fstream fin;
	string line, word, temp;
	kvp_vec kvs;
	pair<string,string> kvp;

	fin.open(fname, ios::in);

	while (fin >> temp && size>0) {
		getline(fin, line);
		int col = 0;
		stringstream s(line);

		while (getline(s, word, ', ') && col<=val_col) {
			if (col == 0)
				kvp.first = word;
			if (col == val_col)
				kvp.second = word;
			++col;
		}
		kvs.push_back(kvp);
		--size;
	}
}

/**
 * Pass in key value pairs by copy and sorts them
 */
kvp_vec copy_sort(const kvp_vec& kvs)
{
	kvp_vec sorted = copy(kvs.begin(),kvs.end());
	sort(sorted.begin(),sorted.end());
	return sorted;
}

/**
 * Generates a random string of length len;
 * used for generating values.
 */
string random_str(const Random &rnd, int len)
{
	string ret(len);

	for (int i=0; i<len; i++) {
		ret[i] = static_cast<char>(' ' + rnd.Uniform(95));  // ' ' .. '~'
	}
	return ret;
}

/**
 * Generates a random key of length len.
 */
string random_key(const Random &rnd, int len)
{
	// Make sure to generate a wide variety of characters so we
	// test the boundary conditions for short-key optimizations.
	static const char kTestChars[] = {'\0', '\1', 'a',    'b',    'c',
									'd',  'e',  '\xfd', '\xfe', '\xff'};
	string ret(len);

	for (int i = 0; i < len; i++) {
		ret[i] += kTestChars[rnd.Uniform(sizeof(kTestChars))];
	}
	return ret;
	
}

/**
 * Generates size number of key value pairs of length len
 * and returns them in a vector.
 */
kvp_vec gen_rand(int seed, int size, int len)
{
	Random rnd(seed);
	kvp_vec kvs;
	pair<string,string> kvp;

	while (size>0){
		kvp.first = random_key(rnd, len);
		kvp.second = random_str(rnd, len);
		kvs.push_back(kvp);
	}
	return kvs;
}

duration<double> clear_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.delete(db);
	auto stop = chrono::steady_clock::now();
	return stop - start;
}

duration<double> write_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.set(pair.first, pair.second);
	auto stop = chrono::steady_clock::now();
	return stop - start;
}

duration<double> seek_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.get(pair.first);
	auto stop = chrono::steady_clock::now();
	return stop - start;
}

struct bench_stat run_bench(ProjectDb& db, int trials, int seed, int size, int len)
{
	kvp_vec kvs = gen_rand(seed, size, len);
	kvp_vec sorted = copy_sorted(kvs);
	struct bench_stats bs;

	write_db(db, sorted);
	clear_db(db, sorted);

	bs.fillseq = write_db(db, sorted);
	clear_db(db, sorted);

	bs.fillrand = write_db(db, kvs);
	clear_db(db, sorted);

	write_db(db, sorted);
	bs.overwrite = write_db(db, sorted);
	clear_db(db, sorted);

	write_db(db, sorted);
	bs.deleteseq = clear_db(db, sorted);

	write_db(db, sorted);
	bs.deleterandom = clear_db(db, kvs);

	write_db(db, sorted);
	bs.seekrandom = seek_db(db, kvs);
	clear_db(db, sorted);

	write_db(db, sorted);
	bs.seekordered = seek_db(db, sorted);
	clear_db(db, sorted);

	return bs;
}

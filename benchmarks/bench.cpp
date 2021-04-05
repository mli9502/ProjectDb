//
// Created by smao on 3/29/21.
//
#include <chrono>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "projectdb/projectdb.h"
#include "bench.h"
#include "random.h"

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

		while (getline(s, word, ',') && col<=val_col) {
			if (col == 0)
				kvp.first = word;
			if (col == val_col)
				kvp.second = word;
			++col;
		}
		kvs.push_back(kvp);
		--size;
	}
	return kvs;
}

/**
 * Directly reads csv data into db.
 */
void csv_db (const string fname, ProjectDb& db, int val_col, int size)
{
	fstream fin;
	string line, temp, word, key, value;

	fin.open(fname, ios::in);

	while (fin >> temp && size>0) {
		getline(fin, line);
		int col = 0;
		stringstream s(line);

		while (getline(s, word, ',') && col<=val_col) {
			if (col == 0)
				key = word;
			if (col == val_col)
				value = word;
			++col;
		}
		db.set(key,word);
		--size;
	}
}

/**
 * Pass in key value pairs by copy and sorts them
 */
kvp_vec copy_sort(const kvp_vec kvs)
{
	kvp_vec sorted = kvs;
	sort(sorted.begin(),sorted.end());
	return sorted;
}

/**
 * Generates a random string of length len;
 * used for generating values.
 */
string random_str(Random &rnd, int len)
{
	string ret;

	for (int i=0; i<len; i++) {
		ret += static_cast<char>(' ' + rnd.Uniform(95));  // ' ' .. '~'
	}
	return ret;
}

/**
 * Generates a random key of length len.
 */
string random_key(Random &rnd, int len)
{
	// Make sure to generate a wide variety of characters so we
	// test the boundary conditions for short-key optimizations.
	static const char kTestChars[] = {'\0', '\1', 'a',    'b',    'c',
									'd',  'e',  '\xfd', '\xfe', '\xff'};
	string ret;

	for (int i = 0; i < len; i++) {
		ret += kTestChars[rnd.Uniform(sizeof(kTestChars))];
	}
	return ret;
	
}

/**
 * Generates size number of key value pairs of length len
 * and returns them in a vector.
 */
kvp_vec gen_rand(int size, int len)
{
	Random rnd(301);
	kvp_vec kvs;
	pair<string,string> kvp;

	while (size>0){
		kvp.first = random_key(rnd, len);
		kvp.second = random_str(rnd, len);
		kvs.push_back(kvp);
		--size;
	}
	return kvs;
}

chrono::duration<double> clear_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.remove(pair.first);
	auto stop = chrono::steady_clock::now();
	return stop - start;
}

chrono::duration<double> write_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.set(pair.first, pair.second);
	auto stop = chrono::steady_clock::now();
	return stop - start;
}

chrono::duration<double> seek_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.get(pair.first);
	auto stop = chrono::steady_clock::now();
	return stop - start;
}

/*
 * Runs the benchmarks described in the header file.
 * The db is cleared after every benchmark to ensure consistency.
 */
struct bench_stats run_bench(ProjectDb& db, int size, int len)
{
	kvp_vec kvs = gen_rand(size, len);
	kvp_vec sorted = copy_sort(kvs);
	struct bench_stats bs;

	bs.fillseq = write_db(db, sorted);
	bs.overwrite = write_db(db, sorted);
	bs.deleteseq = clear_db(db, sorted);
	bs.fillrandom = write_db(db, kvs);
	bs.seekordered = seek_db(db, sorted);
	bs.seekrandom = seek_db(db, kvs);
	bs.deleterandom = clear_db(db, kvs);

//	write_db(db, sorted);
//	clear_db(db, sorted);
//
//	bs.fillseq = write_db(db, sorted);
//	clear_db(db, sorted);
//
//	bs.fillrandom = write_db(db, kvs);
//	clear_db(db, sorted);
//
//	write_db(db, sorted);
//	bs.overwrite = write_db(db, sorted);
//	clear_db(db, sorted);
//
//	write_db(db, sorted);
//	bs.deleteseq = clear_db(db, sorted);
//
//	write_db(db, sorted);
//	bs.deleterandom = clear_db(db, kvs);
//
//	write_db(db, sorted);
//	bs.seekrandom = seek_db(db, kvs);
//	clear_db(db, sorted);
//
//	write_db(db, sorted);
//	bs.seekordered = seek_db(db, sorted);
//	clear_db(db, sorted);

	return bs;
}

void print_stats(struct bench_stats bs)
{

}

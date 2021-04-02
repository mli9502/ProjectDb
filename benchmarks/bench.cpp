//
// Created by smao on 3/29/21.
//
#include <iostream>
#include "memtable_queue.h"
#include "random.h"

using namespace std;
using namespace projectdb;

// Benchmarks modifed from LevelDB
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Benchmarks avaliable:
//       fillseq       -- write N values in sequential key order
//       fillrandom    -- write N values in random key order
//       overwrite     -- overwrite N values in random key order
//       deleteseq     -- delete N keys in sequential order
//       deleterandom  -- delete N keys in random order
//       readseq       -- read N times sequentially
//       readreverse   -- read N times in reverse order
//       readrandom    -- read N times in random order
//       readmissing   -- read N missing keys in random order
//       seekrandom    -- N random seeks
//       seekordered   -- N ordered seeks


/**
 * Prints the keys and values in db
 * given a vector of key and value pairs.
 */
void print_db(MemTableQueue db, vector<string> kvs) 
{
	int size = kvs.size();
	for (int i=0; i<size; i+=2)
		cout<<kvs[i]<<", "<<db.get(kvs[i])<<"\n";
}

/**
 * Reads the first size number of rows from the given csv file 
 * into a vector of key value pairs where the keys are taken from the 
 * first column and the values are taken from val_col.
 */
vector<pair<string,string>> read_csv(string fname, int val_col, int size)
{
	fstream fin;
	string line, word, temp;
	vector<pair<string,string>> kvs;
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
vector<pair<string,string>> copy_sort(const vector<pair<string,string>> kvs)
{
	sort(kvs.begin(),kvs.end());
	return kvs;
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
vector<pair<string,string>> gen_rand(int seed, int size, int len)
{
	Random rnd(seed);
	vector<pair<string,string>> kvs;
	pair<string,string> kvp;

	while (size>0){
		kvp.first = random_key(rnd, len);
		kvp.second = random_str(rnd, len);
		kvs.push_back(kvp);
	}
	return kvs;
}

//
// Created by smao on 3/29/21.
//
#include <chrono>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <filesystem>

#include "projectdb/projectdb.h"
#include "bench.h"
#include "random.h"

using namespace std;
using namespace projectdb;
namespace fs = std::filesystem;

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

pair<string,string> split_csv(const string& line, int val_col)
{
	bool in_quote = 0;
	pair<string,string> kv;
	int start, end;
	int col = 0;
	
	start = end = 0;
	while (col<=val_col) {
		if (line[end]!=',' || in_quote){
			++end;
		} else if (line[end]=='"') {
			in_quote^=1;
		} else if (line[end]==',') {
			if (col == 0)
				kv.first = line.substr(start, end-start);
			if (col == val_col)
				kv.second = line.substr(start, end-start);
			start = ++end;
			++col;
		}
	}
	return kv;
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
	getline(fin, line);

	while (getline(fin, line) && size>0) {
		kvs.push_back(split_csv(line, val_col));
		--size;
	}
	return kvs;
}

/**
 * Directly reads csv data into db.
 */
// void csv_db (const string fname, ProjectDb& db, int val_col, int size)
// {
// 	fstream fin;
// 	string line, temp, word, key, value;
// 
// 	fin.open(fname, ios::in);
// 
// 	while (fin >> temp && size>0) {
// 		getline(fin, line);
// 		int col = 0;
// 		stringstream s(line);
// 
// 		while (getline(s, word, ',') && col<=val_col) {
// 			if (col == 0)
// 				key = word;
// 			if (col == val_col)
// 				value = word;
// 			++col;
// 		}
// 		db.set(key,word);
// 		--size;
// 	}
// }

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
 * Pass in key value pairs and shuffles them
 */
kvp_vec copy_shuf(const kvp_vec kvs)
{
	kvp_vec shuf = kvs;

	random_device rd;
	mt19937 g(rd());

	shuffle(shuf.begin(),shuf.end(),g);
	return shuf;
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
 * Generates size number of key value pairs of length len
 * and returns them in a vector.
 */
kvp_vec gen_rand(int size, int len)
{
	Random rnd(301);
	kvp_vec kvs;
	int key = 0;

	while (key < size){
		kvs.push_back(make_pair(to_string(key),random_str(rnd,len)));
		++key;
	}
	return kvs;
}

chrono::microseconds clear_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.remove(pair.first);
	auto stop = chrono::steady_clock::now();
	return chrono::microseconds((stop-start).count()/kvs.size());
}

chrono::microseconds write_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.set(pair.first, pair.second);
	auto stop = chrono::steady_clock::now();
	return chrono::microseconds((stop-start).count()/kvs.size());
}

chrono::microseconds seek_db (ProjectDb& db, const kvp_vec& kvs)
{
	auto start = chrono::steady_clock::now();
	for (auto pair : kvs)
		db.get(pair.first);
	auto stop = chrono::steady_clock::now();
	return chrono::microseconds((stop-start).count()/kvs.size());
}

/*
 * Runs the benchmarks described in the header file.
 * The db is cleared after every benchmark to ensure consistency.
 */
void run_bench(struct bench_stats& bs, kvp_vec& kvs)
{
	kvp_vec shuf = copy_shuf(kvs);
	fs::remove_all("./projectdb");

	ProjectDb db0;
	bs.fillseq = write_db(db0, kvs);
	bs.overwrite = write_db(db0, kvs);
	bs.deleteseq = clear_db(db0, kvs);

	ProjectDb db1;
	bs.fillrandom = write_db(db1, shuf);
	fs::remove_all("./projectdb");

	ProjectDb db2;
	write_db(db2,kvs);
	bs.seekordered = seek_db(db2, kvs);
	fs::remove_all("./projectdb");

	ProjectDb db3;
	write_db(db3,kvs);
	bs.seekrandom = seek_db(db3, shuf);
	fs::remove_all("./projectdb");

	ProjectDb db4;
	write_db(db4,kvs);
	bs.deleterandom = clear_db(db4, shuf);
	fs::remove_all("./projectdb");
}

void print_chrono(const string b, chrono::microseconds us)
{
	cout<<left<<setw(22)<<b<<us.count()<<'\n';
}

void print_stats(struct bench_stats& bs)
{
	cout<<"Benchmarks in microseconds per operation:\n";
	print_chrono("fillseq", bs.fillseq);
	print_chrono("fillrandom", bs.fillrandom);
	print_chrono("overwrite", bs.overwrite);
	print_chrono("deleteseq", bs.deleteseq);
	print_chrono("deleterandom", bs.deleterandom);
	print_chrono("seekrandom", bs.seekrandom);
	print_chrono("seekordered", bs.seekordered);
}

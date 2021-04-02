//
// Created by smao on 3/29/21.
//
#include <iostream>
#include "memtable_queue.h"
using namespace std;
using namespace projectdb;

/**
 * What handler for db are we using?
 * Which benchmark test do we want?
 * How should we obtain data from leveldb?
 */


/**
 * Prints the keys and values in mq
 * given a vector of keys and values.
 */
void print_(MemTableQueue mq, vector<string> kvs) 
{
	int size = kvs.size();
	for (int i=0; i<size; i+=2)
		cout<<kvs[i]<<", "<<mq.get(kvs[i])<<"\n";
}

/**
 * Reads the first size number of rows from the given csv file 
 * into a vector of strings where the keys are taken from the 
 * first column and the values are taken from val_col.
 */
vector<string> read_csv(string fname, int val_col, int size)
{
	fstream fin;
	string line, word, temp;
	vector<string> kvs(size*2);
	int i = 0;

	fin.open(fname, ios::in);

	while (fin >> temp && size>0) {
		getline(fin, line);
		int col = 0;
		stringstream s(line);

		while (getline(s, word, ', ') && col<=val_col) {
			if (col == 0)
				keys[i++] = word;
			if (col == val_col)
				keys[i++] = word;
			++col;
		}
		--size;
	}
}

string random_str(Random* rnd, size_t)


Slice CompressibleString(Random* rnd, double compressed_fraction, size_t len,
                         std::string* dst) {
  int raw = static_cast<int>(len * compressed_fraction);
  if (raw < 1) raw = 1;
  std::string raw_data;
  RandomString(rnd, raw, &raw_data);

  // Duplicate the random data until we have filled "len" bytes
  dst->clear();
  while (dst->size() < len) {
    dst->append(raw_data);
  }
  dst->resize(len);
  return Slice(*dst);
}

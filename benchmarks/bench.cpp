//
// Created by smao on 3/29/21.
//
#include "bench.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "projectdb/projectdb.h"
#include "random.h"

using namespace std;
using namespace projectdb;
namespace fs = std::filesystem;

using kvp_vec = vector<pair<string, string>>;

optional<pair<string, string>> split_line(const string& line) {
    optional<pair<string, string>> rtn;

    for (string::size_type i = 0; i < line.size(); i++) {
        if (line[i] == ',' && i == 11) {
            rtn = {line.substr(0, i), line.substr(i + 1)};
            break;
        }
    }

    return rtn;
}

/**
 * Reads the first size number of rows from the given csv file
 * into a vector of key value pairs where the keys are taken from the
 * first column and the values are taken from val_col.
 */
kvp_vec read_csv(const string& fname, int size) {
    fstream fin;
    string line, word, temp;
    kvp_vec kvs;
    pair<string, string> kvp;

    fin.open(fname, ios::in);
    getline(fin, line);

    while (getline(fin, line) && size > 0) {
        auto tmp = split_line(line);
        if (tmp.has_value()) {
            kvs.push_back(tmp.value());
        }
        --size;
    }
    return kvs;
}

/**
 * Pass in key value pairs by copy and sorts them
 */
kvp_vec copy_sort(const kvp_vec kvs) {
    kvp_vec sorted = kvs;
    sort(sorted.begin(), sorted.end());
    return sorted;
}

/**
 * Pass in key value pairs and shuffles them
 */
kvp_vec copy_shuf(const kvp_vec kvs) {
    kvp_vec shuf = kvs;

    random_device rd;
    mt19937 g(rd());

    shuffle(shuf.begin(), shuf.end(), g);
    return shuf;
}
/**
 * Generates a random string of length len;
 * used for generating values.
 */
string random_str(Random& rnd, int len) {
    string ret;

    for (int i = 0; i < len; i++) {
        ret += static_cast<char>(' ' + rnd.Uniform(95));  // ' ' .. '~'
    }
    return ret;
}

/**
 * Generates size number of key value pairs of length len
 * and returns them in a vector.
 */
kvp_vec gen_rand(int size, int len) {
    Random rnd(301);
    kvp_vec kvs;
    int key = 0;

    while (key < size) {
        kvs.push_back(make_pair(to_string(key), random_str(rnd, len)));
        ++key;
    }
    return kvs;
}

chrono::duration<double> clear_db(ProjectDb& db, const kvp_vec& kvs) {
    auto start = chrono::steady_clock::now();
    for (auto pair : kvs) db.remove(pair.first);
    auto stop = chrono::steady_clock::now();
    return chrono::duration<double>(stop - start);
}

chrono::duration<double> write_db(ProjectDb& db, const kvp_vec& kvs) {
    auto start = chrono::steady_clock::now();
    for (auto pair : kvs) db.set(pair.first, pair.second);
    auto stop = chrono::steady_clock::now();
    return chrono::duration<double>(stop - start);
}

chrono::duration<double> seek_db(ProjectDb& db, const kvp_vec& kvs) {
    auto start = chrono::steady_clock::now();
    for (auto pair : kvs) db.get(pair.first);
    auto stop = chrono::steady_clock::now();
    return chrono::duration<double>(stop - start);
}

void try_remove_db_dir() {
    fs::path dir("./projectdb");
    if (!fs::exists(dir)) {
        return;
    }
    fs::remove_all(dir);
}

/*
 * Runs the benchmarks described in the header file.
 * The db is cleared after every benchmark to ensure consistency.
 */
void run_bench(struct bench_stats& bs, kvp_vec& kvs) {
    try_remove_db_dir();

    kvp_vec shuf = copy_shuf(kvs);

    {
        ProjectDb db;
        bs.fillseq = write_db(db, kvs);
        bs.overwrite = write_db(db, kvs);
        bs.deleteseq = clear_db(db, kvs);
    }

    try_remove_db_dir();

    {
        ProjectDb db;
        bs.fillrandom = write_db(db, shuf);
    }

    try_remove_db_dir();

    {
        ProjectDb db;
        write_db(db, kvs);
        bs.seekordered = seek_db(db, kvs);
    }

    try_remove_db_dir();

    {
        ProjectDb db;
        write_db(db, kvs);
        bs.seekrandom = seek_db(db, shuf);
    }

    try_remove_db_dir();

    {
        ProjectDb db;
        write_db(db, kvs);
        bs.deleterandom = clear_db(db, shuf);
    }

    try_remove_db_dir();
}

void print_chrono(const string b, const unsigned e,
                  chrono::duration<double> us) {
    cout << left << setw(22) << b << us.count() / e * 1'000'000 << '\n';
}

void print_stats(struct bench_stats& bs) {
    cout << "Benchmarks in microseconds per operation:\n";
    print_chrono("fillseq", bs.entries, bs.fillseq);
    print_chrono("fillrandom", bs.entries, bs.fillrandom);
    print_chrono("overwrite", bs.entries, bs.overwrite);
    print_chrono("deleteseq", bs.entries, bs.deleteseq);
    print_chrono("deleterandom", bs.entries, bs.deleterandom);
    print_chrono("readrandom", bs.entries, bs.readrandom);
    print_chrono("readordered", bs.entries, bs.readordered);
}

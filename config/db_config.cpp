//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#include "db_config.h"

namespace projectdb {

namespace db_config {

unsigned MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;
unsigned SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES;

string DB_FILE_PREFIX;

string DB_FILE_PATH;

string SSTABLE_FILE_EXT;
string SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT;
string MERGED_SSTABLE_FILE_EXT;

string TRANSACTION_LOG_FILE_EXT;

string DEPRECATED_FILE_EXT;

unsigned NUM_SSTABLE_TO_COMPACT;
unsigned SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;

bool KEEP_SSTABLE_FILE_OPEN;

namespace impl {
unsigned SSTABLE_FILE_COUNTER_BASE = 0;
unsigned TRANSACTION_LOG_FILE_COUNTER_BASE = 0;
}  // namespace impl

}  // namespace db_config

}  // namespace projectdb

void read_config() {
    std::ifstream file_in("config");

    using namespace projectdb::db_config;
    string key;
    try {
        while (file_in >> key) {
            // cout << "Read: " << key << " ";
            if (key == "MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES") {
                file_in >> MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;
                // cout << " " << MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES <<
                // endl;
                if (MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES == 0) {
                    throw "WRONG MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES FORMAT";
                    file_in.close();
                }
            } else if (key == "SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES") {
                file_in >> SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES;
                cout << " " << SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES << endl;
                if (SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES == 0) {
                    throw "WRONG SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES FORMAT";
                    file_in.close();
                }
            } else if (key == "DB_FILE_PREFIX") {
                file_in >> DB_FILE_PREFIX;
                cout << " " << DB_FILE_PREFIX << endl;
            } else if (key == "DB_FILE_PATH") {
                file_in >> DB_FILE_PATH;
                cout << " " << DB_FILE_PATH << endl;
            } else if (key == "SSTABLE_FILE_EXT") {
                file_in >> SSTABLE_FILE_EXT;
                cout << " " << SSTABLE_FILE_EXT << endl;
            } else if (key == "SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT") {
                file_in >> SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT;
                cout << " " << SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT << endl;
            } else if (key == "MERGED_SSTABLE_FILE_EXT") {
                file_in >> MERGED_SSTABLE_FILE_EXT;
                cout << " " << MERGED_SSTABLE_FILE_EXT << endl;
            } else if (key == "TRANSACTION_LOG_FILE_EXT") {
                file_in >> TRANSACTION_LOG_FILE_EXT;
                cout << " " << TRANSACTION_LOG_FILE_EXT << endl;
            } else if (key == "DEPRECATED_FILE_EXT") {
                file_in >> DEPRECATED_FILE_EXT;
                cout << " " << DEPRECATED_FILE_EXT << endl;
            } else if (key == "NUM_SSTABLE_TO_COMPACT") {
                file_in >> NUM_SSTABLE_TO_COMPACT;
                cout << " " << NUM_SSTABLE_TO_COMPACT << endl;
                if (NUM_SSTABLE_TO_COMPACT == 0)
                    throw "WRONG NUM_SSTABLE_TO_COMPACT FORMAT";
                file_in.close();
            } else if (key == "SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES") {
                file_in >> SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;
                cout << " " << SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES << endl;
                if (SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES == 0) {
                    throw "WRONG SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES FORMAT";
                    file_in.close();
                }
            } else if (key == "KEEP_SSTABLE_FILE_OPEN") {
                file_in >> KEEP_SSTABLE_FILE_OPEN;
                cout << " " << KEEP_SSTABLE_FILE_OPEN << endl;
            } else {
                throw "UNKOWN CONFIGURATION";
                file_in.close();
            }
        }
    } catch (const char* msg) {
        cerr << msg << endl;
        return;
    }

    file_in.close();
}
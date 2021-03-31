//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#ifndef MAIN_SYSTEM_UTILS_H
#define MAIN_SYSTEM_UTILS_H

#include <chrono>
#include <fstream>

using namespace std;

namespace projectdb {

using timestamp_unit_type = chrono::milliseconds;

timestamp_unit_type getTimeSinceEpoch();

string genSSTableFileName();
string genTransactionLogFileName();

fstream getFileStream(string_view baseFileName, ios_base::openmode ioMode);

}  // namespace projectdb

#endif  // MAIN_SYSTEM_UTILS_H

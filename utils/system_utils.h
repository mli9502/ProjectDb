//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#ifndef MAIN_SYSTEM_UTILS_H
#define MAIN_SYSTEM_UTILS_H

#include <chrono>
#include <filesystem>
#include <fstream>

#include "db_config.h"

using namespace std;

namespace projectdb {

using timestamp_unit_type = chrono::milliseconds;

timestamp_unit_type getTimeSinceEpoch();

string genSSTableFileName();
string genTransactionLogFileName();

string genMergedSSTableFileName(string_view baseFilename);

fstream getFileStream(string_view baseFileName, ios_base::openmode ioMode);

// https://stackoverflow.com/questions/43514665/use-of-auto-funcint-before-deduction-of-auto-in-c14
inline auto getFileSizeInBytes(string_view baseFileName) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    filePath /= baseFileName;
    return filesystem::file_size(filePath);
}

}  // namespace projectdb

#endif  // MAIN_SYSTEM_UTILS_H

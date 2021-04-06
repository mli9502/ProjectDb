//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#ifndef MAIN_SYSTEM_UTILS_H
#define MAIN_SYSTEM_UTILS_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <vector>

#include "db_config.h"

using namespace std;

namespace projectdb {

using timestamp_unit_type = chrono::milliseconds;

timestamp_unit_type getTimeSinceEpoch();

string genSSTableFileName(unsigned ssTableFileCounter);
string genTransactionLogFileName();

string genFlushInProgressSSTableFileName(string_view baseFileName);
string genMergedSSTableFileName(string_view baseFileName);

void initDbPath();

fstream getFileStream(string_view baseFileName, ios_base::openmode ioMode);

void markFileAsDeprecated(string_view baseFileName);

string removeExtAndRename(string_view fileName);

void waitUntilFileExist(string_view fileName);

int getCounterFromFileName(const string& fileName);
vector<string> getFilesWithExtSorted(string_view ext);
void removeFilesWithExt(string_view ext);

// https://stackoverflow.com/questions/43514665/use-of-auto-funcint-before-deduction-of-auto-in-c14
inline auto getFileSizeInBytes(string_view baseFileName) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    filePath /= baseFileName;
    return filesystem::file_size(filePath);
}

}  // namespace projectdb

#endif  // MAIN_SYSTEM_UTILS_H

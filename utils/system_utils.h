//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#ifndef MAIN_SYSTEM_UTILS_H
#define MAIN_SYSTEM_UTILS_H

#include <fstream>

using namespace std;

namespace projectdb {

int getProcessId();

fstream getFileStream(string_view baseFileName, ios_base::openmode ioMode);

}  // namespace projectdb

#endif  // MAIN_SYSTEM_UTILS_H

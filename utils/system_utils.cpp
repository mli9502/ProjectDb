//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#include "system_utils.h"

#include <filesystem>

#include "db_config.h"
#include "log.h"

namespace projectdb {

namespace {
string genFileName(unsigned counter, const string& fileType) {
    return db_config::DB_FILE_PREFIX + "_" + to_string(counter) + "." +
           fileType;
}
}  // namespace

timestamp_unit_type getTimeSinceEpoch() {
    return chrono::duration_cast<timestamp_unit_type>(
        chrono::system_clock::now().time_since_epoch());
}

string genSSTableFileName() {
    static unsigned ssTableFileCounter = 0;
    return genFileName(ssTableFileCounter, db_config::SSTABLE_FILE_TYPE);
}

fstream getFileStream(string_view baseFileName, ios_base::openmode ioMode) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    if ((ioMode & ios::out) != 0) {
        log::debug(
            "out mode, try creating directory to make sure that it exists.");
        try {
            filesystem::create_directories(filePath);
        } catch (const exception& e) {
            log::errorAndThrow(e.what());
        }
    }
    filePath /= baseFileName;
    log::debug("Opening file with path: ", filePath);
    return fstream(filePath, ios::binary | ioMode);
}

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#include "system_utils.h"

#include <thread>

#include "log.h"

namespace projectdb {

namespace {

string genFileName(unsigned counter, const string& fileExt) {
    return db_config::DB_FILE_PREFIX + "_" + to_string(counter) + fileExt;
}

string genDeprecatedFileName(string_view baseFileName) {
    return string(baseFileName) + db_config::DEPRECATED_FILE_EXT;
}

}  // namespace

timestamp_unit_type getTimeSinceEpoch() {
    return chrono::duration_cast<timestamp_unit_type>(
        chrono::system_clock::now().time_since_epoch());
}

// NOTE: @mli: We can't directly use (++
// db_config::impl::SSTABLE_FILE_COUNTER_BASE) in here, because it's possible
// that there are multiple tryFlushToDisk job launched together, which will
// cause a read-modify-write race condition for SSTABLE_FILE_COUNTER_BASE.
string genSSTableFileName(unsigned ssTableFileCounter) {
    //    log::info("ssTableFileCounter: ", ssTableFileCounter);
    return genFileName(ssTableFileCounter, db_config::SSTABLE_FILE_EXT);
}

string genFlushInProgressSSTableFileName(string_view baseFileName) {
    return string(baseFileName) + db_config::SSTABLE_FILE_FLUSH_IN_PROGRESS_EXT;
}

string genMergedSSTableFileName(string_view baseFileName) {
    return string(baseFileName) + db_config::MERGED_SSTABLE_FILE_EXT;
}

string genTransactionLogFileName() {
    return genFileName(db_config::impl::TRANSACTION_LOG_FILE_COUNTER_BASE++,
                       db_config::TRANSACTION_LOG_FILE_EXT);
}

fstream getFileStream(string_view baseFileName, ios_base::openmode ioMode) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    if (((ioMode & ios::out) != 0) || ((ioMode & ios::app) != 0)) {
        try {
            filesystem::create_directories(filePath);
        } catch (const exception& e) {
            log::errorAndThrow("Failed to get file stream for file: ",
                               baseFileName, " with exception: ", e.what());
        }
    }
    filePath /= baseFileName;
    log::debug("Opening file with path: ", filePath);
    return fstream(filePath, ios::binary | ioMode);
}

void markFileAsDeprecated(string_view baseFileName) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    const auto& currFilePath = filePath / baseFileName;
    if (!filesystem::exists(filePath)) {
        log::errorAndThrow("Trying to mark a non-existing file as deprecated: ",
                           filePath);
    }
    const auto& deprecatedFilePath =
        filePath / genDeprecatedFileName(baseFileName);
    filesystem::rename(currFilePath, deprecatedFilePath);
}

/**
 * Removes the last extension of the file, and rename the file with this new
 * name.
 * @param fileName
 * @return the file name with last ext removed. (which is what the original file
 * being renamed to).
 */
string removeExtAndRename(string_view fileName) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    const auto& fullFilePath = filePath / fileName;
    if (!filesystem::exists(fullFilePath)) {
        log::errorAndThrow(
            "Non-existing file path while trying to remove ext and rename: ",
            fullFilePath);
    }
    auto rtn = fullFilePath;
    rtn.replace_extension();
    filesystem::rename(fullFilePath, rtn);
    return rtn.filename().string();
}

void removeDeprecatedFiles() {
    for_each(begin(filesystem::directory_iterator(db_config::DB_FILE_PATH)),
             end(filesystem::directory_iterator(db_config::DB_FILE_PATH)),
             [](auto& p) {
                 if (p.path().extension() == db_config::DEPRECATED_FILE_EXT) {
                     log::debug("Will remove deprecated file: ", p.path());
                     filesystem::remove(p.path());
                 }
             });
}

void waitUntilFileExist(string_view fileName) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    const auto& fullFilePath = filePath / fileName;
    //    log::info("Waiting for: ", fullFilePath);
    while (true) {
        try {
            if (filesystem::exists(fullFilePath)) {
                break;
            }
        } catch (const exception& e) {
            log::debug("Caught exception while waiting for file: [",
                       fullFilePath, "] to be on disk.");
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#include "system_utils.h"

#include "log.h"

namespace projectdb {

namespace {
string genFileName(unsigned counter, const string& fileType) {
    return db_config::DB_FILE_PREFIX + "_" + to_string(counter) + "." +
           fileType;
}

string genDeprecatedFileName(string_view baseFileName) {
    return string(baseFileName) + "." + db_config::DEPRECATED_FILE_TYPE;
}

}  // namespace

timestamp_unit_type getTimeSinceEpoch() {
    return chrono::duration_cast<timestamp_unit_type>(
        chrono::system_clock::now().time_since_epoch());
}

string genSSTableFileName() {
    return genFileName(db_config::impl::SSTABLE_FILE_COUNTER_BASE++,
                       db_config::SSTABLE_FILE_TYPE);
}

string genMergedSSTableFileName(string_view baseFileName) {
    return string(baseFileName) + "." + db_config::MERGED_SSTABLE_FILE_TYPE;
}

string genTransactionLogFileName() {
    return genFileName(db_config::impl::TRANSACTION_LOG_FILE_COUNTER_BASE++,
                       db_config::TRANSACTION_LOG_FILE_TYPE);
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

string markMergedSSTableFileAsActive(string_view mergedFileName) {
    filesystem::path filePath(db_config::DB_FILE_PATH);
    const auto& mergedFilePath = filePath / mergedFileName;
    if (!filesystem::exists(filePath)) {
        log::errorAndThrow(
            "Trying to mark a non-existing merged SSTable file as active: ",
            filePath);
    }
    auto activeFilePath = mergedFilePath;
    activeFilePath.replace_extension();
    filesystem::rename(mergedFilePath, activeFilePath);
    return activeFilePath.filename().string();
}

void removeDeprecatedFiles() {
    for_each(
        begin(filesystem::directory_iterator(db_config::DB_FILE_PATH)),
        end(filesystem::directory_iterator(db_config::DB_FILE_PATH)),
        [](auto& p) {
            if (p.path().extension() == "." + db_config::DEPRECATED_FILE_TYPE) {
                log::debug("Will remove deprecated file: ", p.path());
                filesystem::remove(p.path());
            }
        });
}

}  // namespace projectdb

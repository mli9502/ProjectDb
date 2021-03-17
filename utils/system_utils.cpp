//
// Created by Mengwen Li (ml4643) on 3/16/2021.
//

#include "system_utils.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include <filesystem>

#include "db_config.h"
#include "log.h"

namespace projectdb {

int getProcessId() {
#ifdef __linux__
    return getpid();
#else
    log::info(
        "Including PID in sstable file name not support for system other than "
        "linux for now. Default to 0.");
    return 0;
#endif
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

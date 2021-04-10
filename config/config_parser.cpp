//
// Created by Mengwen Li (ml4643) on 4/10/2021.
//

#include "config_parser.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "db_config.h"
#include "log.h"

namespace projectdb {

namespace {
void setConfig(unsigned& config, const string& value) {
    config = stoul(value);
    log::debug("Converting ", value, " to ", config);
}

void setConfig(bool& config, const string& value) {
    auto intVal = stoi(value);
    if (intVal != 0 && intVal != 1) {
        log::errorAndThrow("Invalid bool value: ", value);
    }
    config = intVal;
    log::debug("Converting ", value, " to ", config);
}

void setConfig(string& config, const string& value) { config = value; }

void updateConfig(const string& config, const string& value) {
    log::debug("Trying to set ", config, " to ", value);
    if (config == "MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES") {
        setConfig(db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES, value);
    } else if (config == "SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES") {
        setConfig(db_config::SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES, value);
    } else if (config == "SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES") {
        setConfig(db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES, value);
    } else if (config == "NUM_SSTABLE_TO_COMPACT") {
        setConfig(db_config::NUM_SSTABLE_TO_COMPACT, value);
    } else if (config == "KEEP_SSTABLE_FILE_OPEN") {
        setConfig(db_config::KEEP_SSTABLE_FILE_OPEN, value);
    } else if (config == "DB_FILE_PATH") {
        setConfig(db_config::DB_FILE_PATH, value);
    } else {
        log::errorAndThrow("Unsupported config found: ", config);
    }
}
}  // namespace

void ConfigParser::parse(const string& configFilePath) {
    if (!filesystem::exists(configFilePath)) {
        log::errorAndThrow("Config file does not exist: ", configFilePath);
    }

    regex r("^([A-Z_]+) (.+)");

    ifstream ifs(configFilePath);
    string line;
    while (getline(ifs, line)) {
        log::debug("Getting line: [", line, "]");
        smatch match;
        if (!regex_search(line, match, r) || (match.size() != 3)) {
            log::debug("Found invalid line: [", line, "], will skip.");
            continue;
        }
        log::debug("Found config: [", match.str(0), "]");
        try {
            updateConfig(match.str(1), match.str(2));
            log::info("Successfully applied config: ", line);
        } catch (const DbException& e) {
            log::debug("Config: [", line, "] is not valid. Will skip.");
        }
    }
}

}  // namespace projectdb

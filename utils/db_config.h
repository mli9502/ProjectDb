//
// Created by Mengwen Li (ml4643) on 3/14/2021.
//

#ifndef MAIN_DB_CONFIG_H
#define MAIN_DB_CONFIG_H

namespace projectdb {

namespace db_config {

/**
 * TODO: @mli: Add a config file parser to set these variables when db starts.
 * json parser maybe?
 */
extern unsigned MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES;

}  // namespace db_config

}  // namespace projectdb

#endif  // MAIN_DB_CONFIG_H

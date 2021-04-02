#include <chrono>
#include <iostream>
#include <sstream>

#include "db_config.h"
#include "key.h"
#include "log.h"
#include "memtable_queue.h"
#include "serializer.h"
#include "sstable.h"
#include "sstable_index.h"
#include "table.h"
#include "transaction_log.h"
#include "value.h"
#include "bench.h"

using namespace std;
using namespace projectdb;

int main() {
    log::debug("Starting database...");

    db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 100;
    db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 50;
    MemTableQueue mq;
    vector<future<SSTableIndex>> futures;
    for (int i = 0; i < 6; i++) {
        auto tmp =
            mq.set(to_string(2 * i), "! Hello World " + to_string(2 * i));
        if (tmp.has_value()) {
            futures.emplace_back(move(tmp.value()));
        }
    }
    int cnt = 0;
    for (auto& ft : futures) {
        while (ft.wait_for(chrono::seconds(0)) != future_status::ready) {
        }
        auto index = ft.get();
        log::debug("Future ", cnt, " ready: ", index);
        cnt += 1;
        Key key("6");
        auto tmp = index.seek(key);
        if (!tmp.has_value()) {
            log::debug(key, " not found!");
        } else {
            log::debug(tmp.value());
        }
        key = Key("8");
        tmp = index.seek(key);
        if (!tmp.has_value()) {
            log::debug(key, " not found!");
        } else {
            log::debug(tmp.value());
        }
        key = Key("1");
        tmp = index.seek(key);
        if (!tmp.has_value()) {
            log::debug(key, " not found!");
        } else {
            log::debug(tmp.value());
        }
        key = Key("2");
        tmp = index.seek(key);
        if (!tmp.has_value()) {
            log::debug(key, " not found!");
        } else {
            log::debug(tmp.value());
        }
        //        auto tmpTable =
        //        SerializationWrapper<Table::mapped_type>().deserialize()
    }

    auto txLog = genTransactionLogFileName();
    auto writter = TransactionLogWritter(txLog);
    writter.write(DbTransactionType::SET, Key("abc"), Value("bcd"));
    writter.write(DbTransactionType::SET, Key("cde"), Value("def"));
    writter.write(DbTransactionType::REMOVE, Key("abc"));
    writter.write(DbTransactionType::SET, Key("efg"), Value("fgh"));

    auto memTable = TransactionLogLoader::load(txLog);
    log::debug(memTable);
    return 0;
}

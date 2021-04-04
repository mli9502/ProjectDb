#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "db_config.h"
#include "key.h"
#include "log.h"
#include "memtable_queue.h"
#include "projectdb/projectdb.h"
#include "serializer.h"
#include "sstable.h"
#include "sstable_index.h"
#include "table.h"
#include "transaction_log.h"
#include "value.h"

using namespace std;
using namespace projectdb;

void test() {
    //    db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 16 * 1024;
    //    db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 16 * 1024;
    //    db_config::NUM_SSTABLE_TO_COMPACT = 2;
    //    db_config::SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 16 * 1024 * 1024;

    db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 100;
    db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 50;
    db_config::NUM_SSTABLE_TO_COMPACT = 2;
    db_config::SSTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 400;

    ProjectDb db;
    for (auto i = 0; i < 5000; i++) {
        db.set(to_string(i), to_string(i) + " Hello World!");
    }
    this_thread::sleep_for(std::chrono::seconds(1));
    for (auto i = 0; i < 5000; i++) {
        if (i % 2 == 0) {
            db.remove(to_string(i));
        }
    }
    this_thread::sleep_for(std::chrono::seconds(1));
    for (auto i = 5000 - 1; i >= 0; i--) {
        db.get(to_string(i));
    }
}

int main() {
    log::debug("Starting database...");
    test();
    //    stringstream ss;
    //    try {
    //        SerializationWrapper<int>(10).serialize(ss);
    //        cout << "After deserialize: "
    //             << SerializationWrapper<int>().deserialize(ss) << endl;
    //        ss.clear();
    //        SerializationWrapper<double>(10.5).serialize(ss);
    //        cout << "After deserialize: "
    //             << SerializationWrapper<double>().deserialize(ss) << endl;
    //        //        Key k{"abc\ntest"};
    //        //        k.serialize(ss);
    //        //        cout << "After deserialize: " << Key().deserialize(ss)
    //        <<
    //        //        endl;
    //    } catch (const DbException& e) {
    //        cout << e.what() << endl;
    //    }
    //
    //    cout << "In here...";

    //    static_assert(serializable_base_trait<int>::value, "Does not support
    //    int"); static_assert(serializable_trait<int>::value, "End for int");
    //    static_assert(serializable_trait<Key>::value, "End for Key");
    //    //    static_assert(serializable_trait<pair<int, int>>::value,
    //    "Pair<int,
    //    //    int>"); static_assert(serializable_trait<pair<pair<int, int>,
    //    //    pair<Key, Value>>>::value, "Pair<int, int>");
    //    static_assert(Serializable<typename map<Key, Value>::value_type>,
    //                  "Pair<int, int>");
    //    static_assert(
    //        Serializable<pair<int, vector<pair<vector<pair<Key, Value>>,
    //        Key>>>>, "Pair<int, vector<int>>");

    //    db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 100;
    //    db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 50;
    //    MemTableQueue mq;
    //    vector<future<SSTableIndex>> futures;
    //    for (int i = 0; i < 6; i++) {
    //        auto tmp =
    //            mq.set(to_string(2 * i), "! Hello World " + to_string(2 * i));
    //        if (tmp.has_value()) {
    //            futures.emplace_back(move(tmp.value()));
    //        }
    //    }
    //    int cnt = 0;
    //    for (auto& ft : futures) {
    //        while (ft.wait_for(chrono::seconds(0)) != future_status::ready) {
    //        }
    //        auto index = ft.get();
    //        log::debug("Future ", cnt, " ready: ", index);
    //        cnt += 1;
    //        Key key("6");
    //        auto tmp = index.seek(key);
    //        if (!tmp.has_value()) {
    //            log::debug(key, " not found!");
    //        } else {
    //            log::debug(tmp.value());
    //        }
    //        key = Key("8");
    //        tmp = index.seek(key);
    //        if (!tmp.has_value()) {
    //            log::debug(key, " not found!");
    //        } else {
    //            log::debug(tmp.value());
    //        }
    //        key = Key("1");
    //        tmp = index.seek(key);
    //        if (!tmp.has_value()) {
    //            log::debug(key, " not found!");
    //        } else {
    //            log::debug(tmp.value());
    //        }
    //        key = Key("2");
    //        tmp = index.seek(key);
    //        if (!tmp.has_value()) {
    //            log::debug(key, " not found!");
    //        } else {
    //            log::debug(tmp.value());
    //        }
    //        //        auto tmpTable =
    //        //        SerializationWrapper<Table::mapped_type>().deserialize()
    //    }
    //
    //    auto txLog = genTransactionLogFileName();
    //    auto writter = TransactionLogWritter(txLog);
    //    writter.write(DbTransactionType::SET, Key("abc"), Value("bcd"));
    //    writter.write(DbTransactionType::SET, Key("cde"), Value("def"));
    //    writter.write(DbTransactionType::REMOVE, Key("abc"));
    //    writter.write(DbTransactionType::SET, Key("efg"), Value("fgh"));
    //
    //    auto memTable = TransactionLogLoader::load(txLog);
    //    log::debug(memTable);
    //
    //    //    db_config::SSTABLE_INDEX_BLOCK_SIZE_IN_BYTES = 0;
    //    //    SSTable sst;
    //    //    SSTableIndex ssti;
    //    //    sst.loadFromDisk("project_db_0_22590.sst", &ssti);
    //    //    log::debug(ssti);
    //    return 0;
}

/*
 * TODO: @mli:
 * Need to consider how should we handle the case the the library is called in
 * a:
 * 1. multi-process way: We probably need to distinguish file name between
 * different processes.
 * 2. multi-thread way: We probably need to provide an option for user to
 * initialize ProjectDb with "forceThreadSafe" as parameter. And we need to add
 * locking around get, set and remove ops to make sure it's thread safe. Maybe
 * create a ThreadSafeDecorator for the operations, which acquires a lock before
 * calling those ops?
 */

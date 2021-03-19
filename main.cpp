#include <chrono>
#include <iostream>
#include <sstream>

#include "db_config.h"
#include "key.h"
#include "log.h"
#include "memtable_queue.h"
#include "serializer.h"
#include "sstable.h"
#include "value.h"

using namespace std;
using namespace projectdb;

int main() {
    log::debug("Starting database...");

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

    db_config::MEMTABLE_APPROXIMATE_MAX_SIZE_IN_BYTES = 100;
    MemTableQueue mq;
    vector<future<SSTableIndex>> futures;
    for (int i = 0; i < 6; i++) {
        auto tmp = mq.set(to_string(i), "Hello World!");
        if (tmp.has_value()) {
            futures.emplace_back(move(tmp.value()));
        }
    }
    int cnt = 0;
    for (auto& ft : futures) {
        while (ft.wait_for(chrono::seconds(0)) != future_status::ready) {
        }
        log::debug("Future ", cnt, " ready: ", ft.get());
        cnt += 1;
    }

    //    SSTable sst;
    //    sst.loadFromDisk("project_db_2_13972.sst");

    return 0;
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

#include <iostream>
#include <sstream>

#include "key.h"
#include "log.h"
#include "serializer.h"
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
    static_assert(serializable_base_trait<int>::value, "Does not support int");
    static_assert(serializable_trait<int>::value, "End for int");
    static_assert(serializable_trait<Key>::value, "End for Key");
    //    static_assert(serializable_trait<pair<int, int>>::value, "Pair<int,
    //    int>"); static_assert(serializable_trait<pair<pair<int, int>,
    //    pair<Key, Value>>>::value, "Pair<int, int>");
    static_assert(Serializable<typename map<Key, Value>::value_type>,
                  "Pair<int, int>");
    static_assert(
        Serializable<pair<int, vector<pair<vector<pair<Key, Value>>, Key>>>>,
        "Pair<int, vector<int>>");

    return 0;
}

#include <iostream>
#include <sstream>

#include "key.h"
#include "log.h"
#include "serializer.h"

using namespace std;
using namespace projectdb;

int main() {
    log::debug("Starting database...");

    stringstream ss;
    try {
        SerializationWrapper<int>(10).serialize(ss);
        cout << "After deserialize: "
             << SerializationWrapper<int>().deserialize(ss) << endl;
        ss.clear();
        SerializationWrapper<double>(10.5).serialize(ss);
        cout << "After deserialize: "
             << SerializationWrapper<double>().deserialize(ss) << endl;
        //        Key k{"abc\ntest"};
        //        k.serialize(ss);
        //        cout << "After deserialize: " << Key().deserialize(ss) <<
        //        endl;
    } catch (const DbException& e) {
        cout << e.what() << endl;
    }

    cout << "In here...";

    return 0;
}

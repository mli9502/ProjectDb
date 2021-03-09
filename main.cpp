#include <iostream>
#include <sstream>

#include "log.h"
#include "serializer.h"

using namespace std;

int main() {
    log::debug("Starting database...");

    stringstream ss;
    try {
        TriviaWrapper<int>(10).serialize(ss);
        cout << "After deserialize: " << TriviaWrapper<int>().deserialize(ss)
             << endl;
        ss.clear();
        TriviaWrapper<double>(10.5).serialize(ss);
        cout << "After deserialize: " << TriviaWrapper<double>().deserialize(ss)
             << endl;
    } catch (const DbException& e) {
        cout << e.what() << endl;
    }

    cout << "In here...";

    return 0;
}

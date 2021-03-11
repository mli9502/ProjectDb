//
// Created by Mengwen Li (ml4643) on 3/6/2021.
//

#ifndef MAIN_LOG_H
#define MAIN_LOG_H

#include <iostream>
#include <sstream>

#include "db_concepts.h"
#include "exception.h"

using namespace std;

namespace projectdb {

namespace log {

namespace impl {

template <Loggable T>
ostringstream& errorAndThrowImpl(ostringstream& oss, T&& arg) {
    oss << arg;
    return oss;
}

template <Loggable T, Loggable... Ts>
ostringstream& errorAndThrowImpl(ostringstream& oss, T&& arg, Ts&&... args) {
    oss << arg;
    return errorAndThrowImpl(oss, forward<Ts>(args)...);
}

}  // namespace impl

using namespace impl;

template <Loggable T>
void debug(T&& arg) {
#ifndef NDEBUG
    cout << arg << endl;
#endif
}

template <Loggable T, Loggable... Ts>
void debug(T&& arg, Ts&&... args) {
#ifndef NDEBUG
    cout << arg;
    debug(forward<Ts>(args)...);
#endif
}

template <Loggable T>
void info(T&& arg) {
    cout << arg << endl;
}

template <Loggable T, Loggable... Ts>
void info(T&& arg, Ts&&... args) {
    cout << arg;
    info(forward<Ts>(args)...);
}

template <Loggable T>
void error(T&& arg) {
    cerr << arg << endl;
}

template <Loggable T, Loggable... Ts>
void error(T&& arg, Ts&&... args) {
    cerr << arg;
    error(forward<Ts>(args)...);
}

template <Loggable... Ts>
void errorAndThrow(Ts&&... args) {
    ostringstream oss;
    errorAndThrowImpl(oss, forward<Ts>(args)...);
    const auto& err = oss.str();
    error(err);
    throw DbException(err);
}

// Utility function to print some common types.
template <Pair T>
ostream& operator<<(ostream& os, const T& t) {
    os << "{" << t.first << ", " << t.second << "}";
    return os;
}

// Cannot just use SerializableContainer as template parameter because of it
// matches string:
// https://stackoverflow.com/questions/54912163/narrowing-down-a-c-concept-to-exclude-certain-types
// https://stackoverflow.com/questions/13724766/how-to-write-a-streaming-operator-that-can-take-arbitary-containers-of-type
// Didn't find a very clean way to overload operator<< for all containers...
// TODO: @mli: Need to add operator<< for separate containers that are used in
// the project...
// TODO: @mli: Need to add tests for deserializing classes.

// template <typename ValueT, template <typename...> typename ContainerT>
// ostream& operator<<(ostream& os, const ContainerT<ValueT>& t) {
//    os << "[";
//    auto idx = 0;
//    for(const auto cit = t.cbegin(); cit != t.cend(); cit ++) {
//        os << *cit;
//        if(idx != t.size() - 1) {
//            os << ", ";
//        }
//        idx ++;
//    }
//    os << "]";
//    return os;
//}

}  // namespace log
}  // namespace projectdb

#endif  // MAIN_LOG_H

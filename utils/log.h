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

}  // namespace log
}  // namespace projectdb

#endif  // MAIN_LOG_H

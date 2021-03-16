//
// Created by Mengwen Li (ml4643) on 3/6/2021.
//

#ifndef MAIN_LOG_H
#define MAIN_LOG_H

#include <iostream>
#include <map>
#include <mutex>
#include <sstream>

#include "db_concepts.h"
#include "exception.h"

using namespace std;

namespace projectdb {

// https://stackoverflow.com/questions/8415890/c-chaining-of-the-operator-for-stdcout-like-usage
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

// Cannot just use SerializableContainer as template parameter because of it
// matches string:
// https://stackoverflow.com/questions/54912163/narrowing-down-a-c-concept-to-exclude-certain-types
// https://stackoverflow.com/questions/13724766/how-to-write-a-streaming-operator-that-can-take-arbitary-containers-of-type
template <typename T>
requires Serializable<T>&& Container<T> class LogWrapper {
   public:
    LogWrapper(const T& value) : m_value(value) {}

    ostream& print(ostream& os) const {
        os << "[";
        size_type idx = 0;
        for (auto it = m_value.cbegin(); it != m_value.cend(); it++) {
            os << *it;
            if (idx != m_value.size() - 1) {
                os << ", ";
            }
            idx++;
        }
        os << "]";
        return os;
    }

   private:
    const T& m_value;
    using size_type = decltype(m_value.size());
};

template <Loggable T>
void debugImpl(T&& arg) {
#ifndef NDEBUG
    cout << arg << endl;
#endif
}

template <Loggable T, Loggable... Ts>
void debugImpl(T&& arg, Ts&&... args) {
#ifndef NDEBUG
    cout << arg;
    debugImpl(forward<Ts>(args)...);
#endif
}

extern mutex syncLog;

}  // namespace impl

using namespace impl;

template <Loggable... Ts>
void debug(Ts&&... args) {
#ifndef NDEBUG
    lock_guard<mutex> guard(syncLog);
    debugImpl(forward<Ts>(args)...);
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

using namespace log::impl;
// Utility function to print some common types.
template <Pair T>
ostream& operator<<(ostream& os, const T& t) {
    os << "{" << t.first << ", " << t.second << "}";
    return os;
}

template <Loggable T>
ostream& operator<<(ostream& os, const vector<T>& value) {
    return LogWrapper<vector<T>>(value).print(os);
}

template <Loggable T, Loggable U>
ostream& operator<<(ostream& os, const map<T, U>& value) {
    return LogWrapper<map<T, U>>(value).print(os);
}

}  // namespace projectdb

#endif  // MAIN_LOG_H

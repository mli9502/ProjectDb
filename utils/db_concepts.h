//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_DB_CONCEPTS_H
#define MAIN_DB_CONCEPTS_H

#include <iostream>
#include <vector>

using namespace std;

namespace projectdb {

template <typename T>
concept Loggable = requires(ostream& os, T t) {
    os << t;
};

// https://en.cppreference.com/w/cpp/types/is_trivial
template <typename T>
concept Trivial = is_trivial<T>::value;

template <typename T>
concept Serializable = requires(T t, ostream& os, istream& is) {
    { t.serialize(os) }
    ->same_as<void>;
    { move(t).deserialize(is) }
    ->same_as<typename T::value_type&&>;
};
}  // namespace projectdb

#endif  // MAIN_DB_CONCEPTS_H

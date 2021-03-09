//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_DB_CONCEPTS_H
#define MAIN_DB_CONCEPTS_H

#include <iostream>
#include <vector>

using namespace std;

template <typename T>
concept Loggable = requires(ostream& os, T t) {
    os << t;
};

// https://en.cppreference.com/w/cpp/types/is_trivial
template <typename T>
concept Trivia = is_trivial<T>::value;

template <typename T>
concept Serializable = requires(T t) {
    { t.toBlob() }
    ->same_as<vector<char>>;
    // TODO: @mli: Add fromBlob().
};

#endif  // MAIN_DB_CONCEPTS_H

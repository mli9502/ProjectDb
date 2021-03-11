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

// TODO: @mli: Need to see if we can test private method.
//              Also, note that SerializationWrapper is NOT Serializable. This
//              is because it returns T::value_type on deserialization instead
//              of T. This kindof make sense because SerializationWrapper should
//              just be a wrapper. It is not actually a class that we want to
//              serialize.
template <typename T>
concept Serializable = requires(T t, ostream& os, istream& is) {
    // T needs to be default constructable.
    { T() }
    ->same_as<T>;
    { move(t).serialize(os) }
    ->same_as<void>;
    { move(t).deserialize(is) }
    ->same_as<T>;
};

// https://stackoverflow.com/questions/60358244/implementing-a-concept-for-pair-with-c20-concepts
template <typename T>
concept Pair = requires(T t) {
    typename T::first_type;
    typename T::second_type;
    { t.first }
    ->same_as<typename T::first_type&>;
    { t.second }
    ->same_as<typename T::second_type&>;
};

template <typename T>
concept SerializableContainer = requires(T t, typename T::iterator it,
                                         typename T::value_type val) {
    typename T::value_type;
    typename T::iterator;
    typename T::const_iterator;
    Trivial<typename T::value_type> || Pair<typename T::value_type> ||
        Serializable<typename T::value_type>;
    { t.begin() }
    ->same_as<typename T::iterator>;
    { t.cbegin() }
    ->same_as<typename T::const_iterator>;
    { t.end() }
    ->same_as<typename T::iterator>;
    { t.cend() }
    ->same_as<typename T::const_iterator>;
    { t.insert(it, val) }
    ->same_as<typename T::iterator>;
    t.size();
};

}  // namespace projectdb

#endif  // MAIN_DB_CONCEPTS_H

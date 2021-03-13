//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_DB_CONCEPTS_H
#define MAIN_DB_CONCEPTS_H

#include <iostream>
#include <vector>

using namespace std;

namespace projectdb {

// https://en.cppreference.com/w/cpp/types/is_trivial
template <typename T>
concept Trivial = is_trivial<T>::value;

template <typename T>
concept SerializableUserDefinedType = requires(T t, ostream& os, istream& is) {
    // T needs to be default constructable.
    { T() }
    ->same_as<T>;
    { move(t).serializeImpl(os) }
    ->same_as<void>;
    { move(t).deserializeImpl(is) }
    ->same_as<T>;
};

/**
 * NOTE: @mli:
 * SerializationWrapper is NOT Serializable.
 * This kind of make sense because SerializationWrapper should just be a
 * wrapper. It is not actually a class that we want to serialize.
 */
template <typename T>
concept SerializableBase = Trivial<T> || SerializableUserDefinedType<T>;

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
concept Container = requires(T t, typename T::iterator it,
                             typename T::value_type val) {
    typename T::value_type;
    typename T::iterator;
    typename T::const_iterator;
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

// https://stackoverflow.com/questions/60358244/implementing-a-concept-for-pair-with-c20-concepts
// https://stackoverflow.com/questions/56741456/how-to-define-a-recursive-concept
// https://stackoverflow.com/questions/35390899/recursive-type-check-using-templates
/**
 * Serializable follows the following recursive definitions:
 *
 * SerializableBase = Trivial |
 *                    SerializableUserDefinedType
 * Serializable = SerializableBase |
 *                Pair<Serializable, Serializable> |
 *                Container<Serializable>
 *
 */
// Define type trait for the basic serializable unit.
template <typename T>
struct serializable_base_trait : std::false_type {};
template <SerializableBase T>
struct serializable_base_trait<T> : std::true_type {};

template <typename T>
struct serializable_trait : serializable_base_trait<T> {};
template <Pair T>
struct serializable_trait<T>
    : conjunction<serializable_trait<remove_const_t<typename T::first_type>>,
                  serializable_trait<typename T::second_type>> {};
template <Container T>
struct serializable_trait<T> : serializable_trait<typename T::value_type> {};

template <typename T>
concept Serializable = serializable_trait<T>::value;
template <typename T>
concept SerializablePair = Pair<T>&& Serializable<T>;
template <typename T>
concept SerializableContainer = Container<T>&& Serializable<T>;

namespace impl {

template <typename T>
concept OstreamLoggable = requires(ostream& os, T t) {
    os << t;
};

}  // namespace impl

template <typename T>
concept Loggable = impl::OstreamLoggable<T> || Serializable<T>;

}  // namespace projectdb

#endif  // MAIN_DB_CONCEPTS_H

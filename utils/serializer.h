//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_SERIALIZER_H
#define MAIN_SERIALIZER_H

#include <array>
#include <concepts>
#include <iostream>
#include <vector>

#include "db_concepts.h"
#include "key.h"
#include "log.h"

using namespace std;

// https://stackoverflow.com/questions/51230764/serialization-deserialization-of-a-vector-of-integers-in-c
// https://isocpp.org/wiki/faq/serialization

namespace projectdb {

/**
 * TODO: @mli: SerializationWrapper need to support a VERSION.
 * This VERSION should also be passed to Serializable::serialize and
 * Serializable::deserialize. Also, VERSION probably is only needed by
 * Serializable types, for all the other std library types and trivial types,
 * they won't change.
 */
// Defination not provided on purpose to prevent using the non-specialized
// version.
// https://stackoverflow.com/questions/7064039/how-to-prevent-non-specialized-template-instantiation
// https://stackoverflow.com/questions/59952704/c20-concepts-which-template-specialization-gets-chosen-when-the-template-arg

// Error throw are done by SerializationWrapper.
// This means that as long as we use SerializationWrapper to
// serialize/deserialize, we don't have to check for os/is.
// SerializationWrapper::serialize only has an rvalue qualifier.
// This is to make sure that we can only call .serialize() with an rvalue
// wrapper, making sure that it's as efficient as possible for the container
// specialization. The container specialization still might need a copy at
// construction depending on whether lvalue or rvalue is used as parameter for
// ctor.

// If we get compile error complaining that SerializationWrapper is undefined,
// it means that we are calling SerializationWrapper on sth that does not match
// any of the specilization below.
template <typename T>
class SerializationWrapper;

template <Trivial T>
class SerializationWrapper<T> {
   public:
    using value_type = T;

    SerializationWrapper() = default;
    explicit SerializationWrapper(T t) : m_t(move(t)){};

    void serialize(ostream& os) && {
        //        log::debug("Serializing Trivial data: ", m_t);
        array<char, sizeof(T)> buf;
        copy(reinterpret_cast<const char*>(&m_t),
             reinterpret_cast<const char*>(&m_t) + sizeof(T), buf.begin());
        os.write(buf.data(), buf.size());
        if (!os) {
            log::errorAndThrow("Failed to serialize trivial data: ", m_t);
        }
    }

    T deserialize(istream& is) && {
        T rtn;
        is.read(reinterpret_cast<char*>(&rtn), sizeof(T));
        if (!is) {
            log::errorAndThrow("Failed to deserialize trivial data!");
        }
        //        log::debug("Successfully deserialized blob into Trivial data:
        //        ", rtn);
        return rtn;
    }

    T m_t{};
};

template <SerializableUserDefinedType T>
class SerializationWrapper<T> {
   public:
    using value_type = T;

    SerializationWrapper() = default;
    explicit SerializationWrapper(T t) : m_t(move(t)) {}

    void serialize(ostream& os) && {
        log::debug("Serializing Serializable data: ", m_t);
        move(m_t).serializeImpl(os);
        if (!os) {
            log::errorAndThrow("Failed to serialize Serializable data!");
        }
    }

    T deserialize(istream& is) && {
        auto rtn = T().deserializeImpl(is);
        if (!is) {
            log::errorAndThrow("Failed to deserialize trivial data!");
        }
        log::debug("Successfully deserialized blob into Serializable data: ",
                   rtn);
        return rtn;
    }

    T m_t{};
};

template <SerializablePair T>
class SerializationWrapper<T> {
   public:
    using value_type = T;

    SerializationWrapper() = default;
    explicit SerializationWrapper(T t) : m_t(move(t)){};

    void serialize(ostream& os) && {
        log::debug("Serializing Pair data: ", m_t);
        // Serialize .first and .second.
        /**
         * NOTE: @mli:
         * We need remove_const for T::first_type.
         * This is because Serializable concept requires serializeImpl and
         * deserializeImpl to be able to be called on non-const types. However,
         * the value_type defined for std::map is std::pair<const Key, T>.
         * (https://en.cppreference.com/w/cpp/container/map) If we don't
         * remove_const here, we will end up with error "invalid use of
         * incomplete type ‘class projectdb::SerializationWrapper<const
         * projectdb::Key>’" because the most generic version of
         * SerializationWrapper<T> is not defined. It is also safe to cast away
         * const in here.
         */
        SerializationWrapper<
            typename remove_const<typename T::first_type>::type>(
            move(m_t.first))
            .serialize(os);
        SerializationWrapper<typename T::second_type>(move(m_t.second))
            .serialize(os);
    }

    T deserialize(istream& is) && {
        // Deserialize .first and .second.
        typename T::first_type first =
            SerializationWrapper<
                typename remove_const<typename T::first_type>::type>()
                .deserialize(is);
        typename T::second_type second =
            SerializationWrapper<typename T::second_type>().deserialize(is);
        T rtn{first, second};
        log::debug("Successfully deserialized blob into Pair data: ", rtn);
        return rtn;
    }

    T m_t{};
};

// TODO: @mli: Test if it's possible to deserialize a given stream of value_type
// without knowing the size. This could be needed if we want to just load part
// of the sstable into memory, given that we have index.
template <SerializableContainer T>
class SerializationWrapper<T> {
   public:
    using value_type = T;

    SerializationWrapper() = default;
    explicit SerializationWrapper(T t) : m_t(move(t)){};

    void serialize(ostream& os) && {
        log::debug("Serializing SerializableContainer data: ", m_t);
        SerializationWrapper<size_type>(m_t.size()).serialize(os);
        // Then, serialize each element in container.
        for (auto it = m_t.begin(); it != m_t.end(); it++) {
            SerializationWrapper<container_value_type>(move(*it)).serialize(os);
        }
    }

    template <typename InvocableT>
    requires IndexBuilderInvocable<InvocableT, T> void serialize(
        ostream& os, InvocableT tryBuildIndex) && {
        log::debug("Serializing SerializableContainer data: ", m_t);
        SerializationWrapper<size_type>(m_t.size()).serialize(os);
        streamsize currBlockSize = 0;
        auto prevPos = os.tellp();
        auto currPos = prevPos;
        // Then, serialize each element in container.
        for (auto it = m_t.begin(); it != m_t.end(); it++) {
            bool isFirstOrLastEntry =
                ((it == m_t.begin()) || (distance(it, m_t.end()) == 1));
            bool indexAdded =
                tryBuildIndex(*it, currPos, currBlockSize, isFirstOrLastEntry);
            // Reset the block size accumulator if we just add an index.
            if (indexAdded) {
                currBlockSize = 0;
            }
            SerializationWrapper<container_value_type>(move(*it)).serialize(os);
            prevPos = currPos;
            currPos = os.tellp();
            currBlockSize += currPos - prevPos;
        }
    }

    T deserialize(istream& is) && {
        T rtn;
        size_type size = SerializationWrapper<size_type>().deserialize(is);
        for (size_type i = 0; i < size; i++) {
            rtn.insert(
                rtn.end(),
                SerializationWrapper<container_value_type>().deserialize(is));
        }
        log::debug(
            "Successfully deserialized blob into SerializableContainer data: ",
            rtn);
        return rtn;
    }

    template <typename InvocableT>
    requires IndexBuilderInvocable<InvocableT, T> T
    deserialize(istream& is, InvocableT tryBuildIndex) && {
        T rtn;
        size_type size = SerializationWrapper<size_type>().deserialize(is);
        streamsize currBlockSize = 0;
        auto prevPos = is.tellg();
        auto currPos = prevPos;
        for (size_type i = 0; i < size; i++) {
            rtn.insert(
                rtn.end(),
                SerializationWrapper<container_value_type>().deserialize(is));
            bool isFirstOrLastEntry = ((i == 0) || (i == size - 1));
            prevPos = currPos;
            currPos = is.tellg();
            currBlockSize += currPos - prevPos;
            bool indexAdded = tryBuildIndex(*(rtn.rbegin()), prevPos,
                                            currBlockSize, isFirstOrLastEntry);
            if (indexAdded) {
                currBlockSize = 0;
            }
        }
        log::debug(
            "Successfully deserialized blob into SerializableContainer data: ",
            rtn);
        return rtn;
    }

    T deserialize(istream& is, ios::pos_type start, ios::pos_type end) {
        T rtn;
        is.seekg(start);
        auto currPos = start;
        while (currPos < end) {
            rtn.insert(
                rtn.end(),
                SerializationWrapper<container_value_type>().deserialize(is));
            currPos = is.tellg();
        }
        log::debug("Successfully deserialized entries in pos range [", start,
                   ", ", end, "): ", rtn);
        return rtn;
    }

    // TODO: @mli: Add deserialize several entries to a container.

    T m_t{};

   private:
    using container_value_type = typename T::value_type;
    using size_type = decltype(m_t.size());
};

}  // namespace projectdb

#endif  // MAIN_SERIALIZER_H

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_SERIALIZER_H
#define MAIN_SERIALIZER_H

#include <array>
#include <concepts>
#include <iostream>
#include <variant>
#include <vector>

#include "db_concepts.h"
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

namespace impl {

template <typename T>
class SerializationWrapperBase {
   public:
    using value_type = T;

   protected:
    explicit SerializationWrapperBase(const T& t) : m_t(&t) {}
    explicit SerializationWrapperBase(T&& t) : m_t(move(t)) {}
    // https://www.codesynthesis.com/~boris/blog/2012/07/24/const-rvalue-references/
    // Prevent const rvalue ref to accidentally bind to const T& ctor.
    explicit SerializationWrapperBase(const T&& t) = delete;

    const T& getCRefT() const {
        if (holds_alternative<const T*>(m_t)) {
            return *(get<const T*>(m_t));
        } else {
            return get<const T>(m_t);
        }
    }

   private:
    const variant<const T*, const T> m_t;
};

}  // namespace impl

template <typename T>
class SerializationWrapper : public impl::SerializationWrapperBase<T> {
    static_assert(sizeof(T) == -1,
                  "No specialization for SerializationWrapper found for type!");
};
template <typename T>
class DeserializationWrapper {
    static_assert(
        sizeof(T) == -1,
        "No specialization for DeserializationWrapper found for type!");
};

template <Trivial T>
class SerializationWrapper<T> : public impl::SerializationWrapperBase<T> {
   public:
    // NOTE: @mli: We can't pass forward<const T>(t) to
    // SerializationWrapperBase, since forward is just static_cast<T&&>, as a
    // result, it only makes sense for forwarding a universal reference.
    // https://stackoverflow.com/questions/8526598/how-does-stdforward-work
    // In this use case, T is already deducted together with the class
    // instantiation.
    explicit SerializationWrapper(const T& t)
        : impl::SerializationWrapperBase<T>(t){};
    explicit SerializationWrapper(T&& t)
        : impl::SerializationWrapperBase<T>(move(t)) {}
    /**
     * Serializes the wrapper value to out stream "os".
     */
    void operator()(ostream& os) && {
        const T& t = this->getCRefT();
        array<char, sizeof(T)> buf;
        copy(reinterpret_cast<const char*>(&t),
             reinterpret_cast<const char*>(&t) + sizeof(T), buf.begin());
        os.write(buf.data(), buf.size());
        if (!os) {
            throw DbException("Invalid stream encountered!");
        }
    }
};

template <Trivial T>
class DeserializationWrapper<T> {
   public:
    using value_type = T;

    /**
     * Deserializes the in stream "is" and return its value.
     */
    T operator()(istream& is) && {
        T rtn;
        is.read(reinterpret_cast<char*>(&rtn), sizeof(T));
        if (!is) {
            throw DbException("Invalid stream encountered!");
        }
        return rtn;
    }
};

template <SerializableUserDefinedType T>
class SerializationWrapper<T> : public impl::SerializationWrapperBase<T> {
   public:
    explicit SerializationWrapper(const T& t)
        : impl::SerializationWrapperBase<T>(t){};
    explicit SerializationWrapper(T&& t)
        : impl::SerializationWrapperBase<T>(move(t)) {}

    void operator()(ostream& os) && {
        const T& t = this->getCRefT();
        //        log::debug("Serializing Serializable data: ", t);
        t.serializeImpl(os);
        if (!os) {
            log::errorAndThrow("Failed to serialize Serializable data!");
        }
    }
};

template <SerializableUserDefinedType T>
class DeserializationWrapper<T> {
   public:
    using value_type = T;

    T operator()(istream& is) && {
        auto rtn = T().deserializeImpl(is);
        if (!is) {
            log::errorAndThrow("Failed to deserialize trivial data!");
        }
        //        log::debug("Successfully deserialized blob into Serializable
        //        data: ", rtn);
        return rtn;
    }
};

template <SerializablePair T>
class SerializationWrapper<T> : public impl::SerializationWrapperBase<T> {
   public:
    explicit SerializationWrapper(const T& t)
        : impl::SerializationWrapperBase<T>(t){};
    explicit SerializationWrapper(T&& t)
        : impl::SerializationWrapperBase<T>(move(t)) {}

    void operator()(ostream& os) && {
        const T& t = this->getCRefT();
        //        log::debug("Serializing Pair data: ", t);
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
            typename remove_const<typename T::first_type>::type>(t.first)(os);
        SerializationWrapper<typename T::second_type>(t.second)(os);
    }
};

template <SerializablePair T>
class DeserializationWrapper<T> {
   public:
    using value_type = T;

    T operator()(istream& is) && {
        // Deserialize .first and .second.
        typename T::first_type first = DeserializationWrapper<
            typename remove_const<typename T::first_type>::type>()(is);
        typename T::second_type second =
            DeserializationWrapper<typename T::second_type>()(is);
        T rtn{first, second};
        //        log::debug("Successfully deserialized blob into Pair data: ",
        //        rtn);
        return rtn;
    }
};

template <SerializableContainer T>
class SerializationWrapper<T> : public impl::SerializationWrapperBase<T> {
   public:
    explicit SerializationWrapper(const T& t)
        : impl::SerializationWrapperBase<T>(t){};
    explicit SerializationWrapper(T&& t)
        : impl::SerializationWrapperBase<T>(move(t)) {}

    void operator()(ostream& os) && {
        const T& t = this->getCRefT();
        //        log::debug("Serializing SerializableContainer data: ", t);
        SerializationWrapper<size_type>(t.size())(os);
        // Then, serialize each element in container.
        for (auto it = t.begin(); it != t.end(); it++) {
            // NOTE: @mli: Can't use (*it) here because of most vexing parse.
            SerializationWrapper<container_value_type>{*it}(os);
        }
    }

    template <typename InvocableT>
    requires IndexBuilderInvocable<InvocableT, T> void operator()(
        ostream& os, InvocableT tryBuildIndex) && {
        const T& t = this->getCRefT();
        //        log::debug("Serializing SerializableContainer data: ", t);
        SerializationWrapper<size_type>(t.size())(os);
        streamsize currBlockSize = 0;
        auto prevPos = os.tellp();
        auto currPos = prevPos;
        // Then, serialize each element in container.
        for (auto it = t.begin(); it != t.end(); it++) {
            bool isFirstOrLastEntry =
                ((it == t.begin()) || (distance(it, t.end()) == 1));
            bool indexAdded =
                tryBuildIndex(*it, currPos, currBlockSize, isFirstOrLastEntry);
            // Reset the block size accumulator if we just add an index.
            if (indexAdded) {
                currBlockSize = 0;
            }
            SerializationWrapper<container_value_type>{*it}(os);
            prevPos = currPos;
            currPos = os.tellp();
            currBlockSize += currPos - prevPos;
        }
    }

   private:
    using container_value_type = typename T::value_type;
    using size_type = typename T::size_type;
};

template <SerializableContainer T>
class DeserializationWrapper<T> {
   public:
    using value_type = T;

    T operator()(istream& is) && {
        T rtn;
        size_type size = DeserializationWrapper<size_type>{}(is);
        for (size_type i = 0; i < size; i++) {
            rtn.insert(rtn.end(),
                       DeserializationWrapper<container_value_type>{}(is));
        }
        //        log::debug("Successfully deserialized blob into
        //        SerializableContainer data: ", rtn);
        return rtn;
    }

    template <typename InvocableT>
    requires IndexBuilderInvocable<InvocableT, T> T
    operator()(istream& is, InvocableT tryBuildIndex) && {
        T rtn;
        size_type size = DeserializationWrapper<size_type>{}(is);
        streamsize currBlockSize = 0;
        auto prevPos = is.tellg();
        auto currPos = prevPos;
        for (size_type i = 0; i < size; i++) {
            rtn.insert(rtn.end(),
                       DeserializationWrapper<container_value_type>{}(is));
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
        //        log::debug("Successfully deserialized blob into
        //        SerializableContainer data: ", rtn);
        return rtn;
    }

    T operator()(istream& is, ios::pos_type start, ios::pos_type end) {
        T rtn;
        is.seekg(start);
        auto currPos = start;
        while (currPos < end) {
            rtn.insert(rtn.end(),
                       DeserializationWrapper<container_value_type>{}(is));
            currPos = is.tellg();
        }
        //        log::debug("Successfully deserialized entries in pos range [",
        //        start, ", ", end, "): ", rtn);
        return rtn;
    }

   private:
    using container_value_type = typename T::value_type;
    using size_type = typename T::size_type;
};

}  // namespace projectdb

#endif  // MAIN_SERIALIZER_H

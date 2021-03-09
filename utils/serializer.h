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
#include "log.h"

using namespace std;

// https://stackoverflow.com/questions/51230764/serialization-deserialization-of-a-vector-of-integers-in-c
// https://isocpp.org/wiki/faq/serialization

template <Trivia T>
class TriviaWrapper {
   public:
    TriviaWrapper() = default;
    explicit TriviaWrapper(T t) : m_t(move(t)){};

    void serialize(ostream& os) const {
        array<char, sizeof(T)> buf;
        copy(reinterpret_cast<const char*>(&m_t),
             reinterpret_cast<const char*>(&m_t) + sizeof(T), buf.begin());
        os.write(buf.data(), buf.size());
        if (!os) {
            log::errorAndThrow("Failed to serialize trivia data: ", m_t);
        }
    }

    T&& deserialize(istream& is) && {
        is.read(reinterpret_cast<char*>(&m_t), sizeof(T));
        if (!is) {
            log::errorAndThrow("Failed to deserialize trivia data!");
        }
        return move(m_t);
    }

   private:
    T m_t{};
};

// class Serializer {
//   public:
//    template <Serializable T>
//    Serializer&& append(const T& obj) && {
//        const vector<char>& blob = obj.toBlob();
//        appendSize(blob.size());
//        m_blob.insert(m_blob.end(), blob.begin(), blob.end());
//        return move(*this);
//    }
//
//    vector<char>&& getBlob() &&;
//
//   private:
//    vector<char> m_blob;
//
//    void appendSize(int size);
//};

#endif  // MAIN_SERIALIZER_H

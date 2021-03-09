//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_SERIALIZER_H
#define MAIN_SERIALIZER_H

#include <concepts>
#include <vector>

#include "db_concepts.h"

using namespace std;

// template <integral T>

// https://stackoverflow.com/questions/51230764/serialization-deserialization-of-a-vector-of-integers-in-c

class Serializer {
   public:
    template <Serializable T>
    Serializer&& append(const T& obj) && {
        const vector<char>& blob = obj.toBlob();
        appendSize(blob.size());
        m_blob.insert(m_blob.end(), blob.begin(), blob.end());
        return move(*this);
    }

    vector<char>&& getBlob() &&;

   private:
    vector<char> m_blob;

    void appendSize(int size);
};

#endif  // MAIN_SERIALIZER_H

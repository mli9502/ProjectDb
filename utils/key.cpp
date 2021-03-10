//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "key.h"

#include <algorithm>

#include "serializer.h"

namespace projectdb {

Key::Key(string key) : m_key(move(key)) {}

string Key::key() const { return m_key; }

void Key::serialize(ostream& os) const {
    // First serialize the size of the string.
    TrivialWrapper<size_type>(m_key.size()).serialize(os);
    // Then, for each char of the string, serialize the char.
    for_each(m_key.begin(), m_key.end(),
             [&](char c) { TrivialWrapper<char>(c).serialize(os); });
}

Key Key::deserialize(istream& is) && {
    // First read the size of the string.
    auto size = TrivialWrapper<size_type>().deserialize(is);
    log::debug("size of key: ", size);
    m_key = string(size, ' ');
    for (size_type i = 0; i < size; i++) {
        m_key.at(i) = TrivialWrapper<char>().deserialize(is);
    }
    return move(*this);
}

ostream& operator<<(ostream& os, const Key& key) {
    os << "[" << key.m_key << "]";
    return os;
}

}  // namespace projectdb

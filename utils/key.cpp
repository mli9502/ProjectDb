//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "key.h"

#include <algorithm>

#include "serializer.h"

namespace projectdb {

Key::Key(string key) : m_key(move(key)) {}

string Key::key() const { return m_key; }

void Key::serialize(ostream& os) && {
    SerializationWrapper<string>(move(m_key)).serialize(os);
}

Key Key::deserialize(istream& is) && {
    SerializationWrapper<string>().deserialize(is);
    return move(*this);
}

ostream& operator<<(ostream& os, const Key& key) {
    os << "[" << key.m_key << "]";
    return os;
}

}  // namespace projectdb

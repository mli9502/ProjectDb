//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "key.h"

#include <algorithm>

#include "serializer.h"

namespace projectdb {

Key::Key(value_type key) : m_key(move(key)) {}

Key::value_type Key::key() const { return m_key; }

void Key::serializeImpl(ostream& os) && {
    SerializationWrapper<value_type>(move(m_key)).serialize(os);
}

Key Key::deserializeImpl(istream& is) && {
    m_key = SerializationWrapper<value_type>().deserialize(is);
    return move(*this);
}

unsigned Key::getApproximateSizeInBytes() const {
    return m_key.size() * sizeof(value_type::value_type);
}

ostream& operator<<(ostream& os, const Key& key) {
    os << "{ m_key: [" << key.m_key << "] }";
    return os;
}

bool operator==(const Key& lhs, const Key& rhs) {
    return lhs.m_key == rhs.m_key;
}

}  // namespace projectdb

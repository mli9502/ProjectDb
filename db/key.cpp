//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "key.h"

#include <algorithm>

#include "serializer.h"

namespace projectdb {

Key::Key(value_type key) : m_key(move(key)) {}

Key::value_type Key::key() const { return m_key; }

/**
 * Calls the wrapper to serializes the key to out stream "os".
 */
void Key::serializeImpl(ostream& os) const& {
    SerializationWrapper<value_type>{m_key}(os);
}

/**
 * Calls the wrapper to deserialize the key itself from in stream "is",
 * set it for its class Key, and return its class Key.
 */
Key Key::deserializeImpl(istream& is) && {
    m_key = DeserializationWrapper<value_type>{}(is);
    return move(*this);
}

unsigned Key::getApproximateSizeInBytes() const {
    return m_key.size() * sizeof(value_type::value_type);
}

bool Key::operator<(const Key& rhs) const { return m_key < rhs.m_key; }

ostream& operator<<(ostream& os, const Key& key) {
    os << "{ m_key: [" << key.m_key << "] }";
    return os;
}

bool operator==(const Key& lhs, const Key& rhs) {
    return lhs.m_key == rhs.m_key;
}

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "value.h"

#include "log.h"
#include "serializer.h"

namespace projectdb {

Value::Value() : m_type(Type::TOMBSTONE_VALUE) {}

Value::Value(value_type value)
    : m_type(Type::STRING_VALUE), m_value(move(value)) {}

bool Value::isTombstoneValue() const { return m_type == Type::TOMBSTONE_VALUE; }

Value::value_type Value::value() const {
    if (isTombstoneValue()) {
        log::errorAndThrow("Trying to access value for TOMBSTONE!");
    }
    return m_value;
}

void Value::serializeImpl(ostream& os) const& {
    SerializationWrapper<Value::Type>{m_type}(os);
    SerializationWrapper<value_type>{m_value}(os);
}

Value Value::deserializeImpl(istream& is) && {
    m_type = DeserializationWrapper<Value::Type>{}(is);
    m_value = DeserializationWrapper<value_type>{}(is);
    return move(*this);
}

unsigned Value::getApproximateSizeInBytes() const {
    return sizeof(m_type) + (m_value.size() * sizeof(value_type::value_type));
}

ostream& operator<<(ostream& os, const Value::Type& type) {
    if (type == Value::Type::STRING_VALUE) {
        os << "STRING_VALUE";
    } else {
        os << "TOMBSTONE_VALUE";
    }
    return os;
}

/**
 * TODO: @mli:
 * Consider using formatter so the format can be typed more cleanly?
 */
ostream& operator<<(ostream& os, const Value& value) {
    os << "{ m_type: [" << value.m_type << "], m_value: [" << value.m_value
       << "] }";
    return os;
}

bool operator==(const Value& lhs, const Value& rhs) {
    return (lhs.m_type == rhs.m_type) && (lhs.m_value == rhs.m_value);
}

}  // namespace projectdb

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "value.h"

#include "log.h"

Value::Value() : m_type(Type::TOMBSTONE_VALUE) {}

Value::Value(string value) : m_type(Type::STRING_VALUE), m_value(move(value)) {}

string Value::value() const {
    if (m_type == Type::TOMBSTONE_VALUE) {
        log::errorAndThrow("Trying to access value for TOMBSTONE!");
    }
    return m_value;
}

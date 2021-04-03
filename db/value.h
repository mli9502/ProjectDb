//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_VALUE_H
#define MAIN_VALUE_H

#include <iostream>
#include <optional>
#include <string>

using namespace std;

namespace projectdb {

/**
 * The class Value represents the value corresponding to a Key in the database.
 * The actual value itself is held in the class
 * and supports serialization to disk and deseralization from disk.
 * It also holds a TOMBSTONE_VALUE for removed/empty values.
 */
class Value {
   public:
    using value_type = string;

    enum class Type { STRING_VALUE, TOMBSTONE_VALUE };

    // Ctor.
    Value();  // Constructs a TOMBSTONE_VALUE.
    explicit Value(string value);

    [[nodiscard]] bool isTombstoneValue() const;
    [[nodiscard]] optional<value_type> underlyingValue() const;

    void serializeImpl(ostream& os) const&;
    Value deserializeImpl(istream& is) &&;

    [[nodiscard]] unsigned getApproximateSizeInBytes() const;

    friend ostream& operator<<(ostream& os, const Value& value);
    friend bool operator==(const Value& lhs, const Value& rhs);

   private:
    Type m_type;
    value_type m_value;
};

ostream& operator<<(ostream& os, const Value::Type& type);
ostream& operator<<(ostream& os, const Value& value);
bool operator==(const Value& lhs, const Value& rhs);

}  // namespace projectdb

#endif  // MAIN_VALUE_H

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_VALUE_H
#define MAIN_VALUE_H

#include <iostream>
#include <string>

using namespace std;

namespace projectdb {

class Value {
   public:
    enum class Type { STRING_VALUE, TOMBSTONE_VALUE };

    // Ctor.
    Value();  // Constructs a TOMBSTONE_VALUE.
    explicit Value(string value);

    [[nodiscard]] string value() const;

    void serializeImpl(ostream& os) &&;
    Value deserializeImpl(istream& is) &&;

    friend ostream& operator<<(ostream& os, const Value& value);
    friend bool operator==(const Value& lhs, const Value& rhs);

   private:
    Type m_type;
    string m_value;
};

ostream& operator<<(ostream& os, const Value::Type& type);
ostream& operator<<(ostream& os, const Value& value);
bool operator==(const Value& lhs, const Value& rhs);

}  // namespace projectdb

#endif  // MAIN_VALUE_H

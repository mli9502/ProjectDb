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

   private:
    Type m_type;
    string m_value;
};

// Provide logging for Value::Type.
ostream& operator<<(ostream& os, const Value::Type& t) {
    os << static_cast<underlying_type<Value::Type>::type>(t);
    return os;
}

}  // namespace projectdb

#endif  // MAIN_VALUE_H

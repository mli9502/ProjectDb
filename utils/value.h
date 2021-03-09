//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_VALUE_H
#define MAIN_VALUE_H

#include <string>

using namespace std;

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

#endif  // MAIN_VALUE_H

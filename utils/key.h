//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_KEY_H
#define MAIN_KEY_H

#include <string>

using namespace std;

namespace projectdb {

class Key {
   public:
    using value_type = Key;
    using size_type = string::size_type;

    // Ctor.
    Key() = default;
    explicit Key(string key);

    ~Key() = default;

    // Copy ctor and move ctor.
    Key(const Key& key) = default;

    Key(Key&& key) = default;

    // Copy assignment and move assignment.
    Key& operator=(const Key& key) = default;

    Key& operator=(Key&& key) = default;

    [[nodiscard]] string key() const;

    void serialize(ostream& os) const;
    Key deserialize(istream& is) &&;

    friend ostream& operator<<(ostream& os, const Key& key);

   private:
    string m_key;
};

ostream& operator<<(ostream& os, const Key& key);

// TODO: @mli: Define hashing for key.

}  // namespace projectdb

#endif  // MAIN_KEY_H

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

    // TODO: @mli: serialize and deserialize should be make private, and
    // SerializationWrapper should be friend class of this.
    //              This is to make sure that no one accidentially call
    //              serialize/deserialize accidentally.
    void serialize(ostream& os) &&;
    Key deserialize(istream& is) &&;

    friend ostream& operator<<(ostream& os, const Key& key);

   private:
    string m_key;
};

ostream& operator<<(ostream& os, const Key& key);

// TODO: @mli: Define hashing for key.

}  // namespace projectdb

#endif  // MAIN_KEY_H

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

    /**
     * NOTE: @mli:
     * I want to make sure that these two methods can't be called directly.
     * And serialization/deserialization must be done through
     * SerializationWrapper due to the additional error checking. However, I
     * didn't find a good way to handle this. Initially, I want to make
     * serializeImpl and deserializeImpl private, and make SerializationWrapper
     * a friend class. But, the use of Serializable concept requires method to
     * be public. So, for now, just make the name <>Impl, hopefully indicate
     * that these should not be called directly.
     */
    void serializeImpl(ostream& os) &&;
    Key deserializeImpl(istream& is) &&;

    friend ostream& operator<<(ostream& os, const Key& key);
    friend bool operator==(const Key& lhs, const Key& rhs);

   private:
    string m_key;
};

ostream& operator<<(ostream& os, const Key& key);
bool operator==(const Key& lhs, const Key& rhs);

// TODO: @mli: Define hashing for key.

}  // namespace projectdb

#endif  // MAIN_KEY_H

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#ifndef MAIN_KEY_H
#define MAIN_KEY_H

#include <string>

using namespace std;

namespace projectdb {
/**
 * The class Key represents a key in the database.
 * The key itself is a string and the class supports serialization to disk and
 * deseralization from disk.
 */
class Key {
   public:
    using value_type = string;
    using size_type = string::size_type;

    // Ctor.
    Key() = default;
    explicit Key(value_type key);

    ~Key() = default;

    // Copy ctor and move ctor.
    Key(const Key& key) = default;

    Key(Key&& key) = default;

    // Copy assignment and move assignment.
    Key& operator=(const Key& key) = default;

    Key& operator=(Key&& key) = default;

    [[nodiscard]] value_type key() const;

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
    void serializeImpl(ostream& os) const&;
    Key deserializeImpl(istream& is) &&;

    [[nodiscard]] unsigned getApproximateSizeInBytes() const;

    friend ostream& operator<<(ostream& os, const Key& key);
    friend bool operator==(const Key& lhs, const Key& rhs);

    friend class std::less<Key>;

   private:
    value_type m_key;
};

ostream& operator<<(ostream& os, const Key& key);
bool operator==(const Key& lhs, const Key& rhs);

}  // namespace projectdb

// Choose between operator< and specialize std::less:
// https://stackoverflow.com/questions/1102392/how-can-i-use-stdmaps-with-user-defined-types-as-key
namespace std {

using namespace projectdb;

template <>
struct less<Key> {
    /**
     * Comparison for the class Key that uses the key itself.
     */
    bool operator()(const Key& lhs, const Key& rhs) const {
        return lhs.m_key < rhs.m_key;
    }
};

}  // namespace std

#endif  // MAIN_KEY_H

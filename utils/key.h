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
    // Ctor.
    explicit Key(string key);

    ~Key() = default;

    // Copy ctor and move ctor.
    Key(const Key& key) = default;

    Key(Key&& key) = default;

    // Copy assignment and move assignment.
    Key& operator=(const Key& key) = default;

    Key& operator=(Key&& key) = default;

    [[nodiscard]] string key() const;

   private:
    string m_key;
};
}  // namespace projectdb

#endif  // MAIN_KEY_H

//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "key.h"

Key::Key(string key) : m_key(move(key)) {}

string Key::key() const { return m_key; }

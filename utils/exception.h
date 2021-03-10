//
// Created by Mengwen Li (ml4643) on 3/6/2021.
//

#ifndef MAIN_EXCEPTION_H
#define MAIN_EXCEPTION_H

#include <exception>
#include <string>

using namespace std;

namespace projectdb {

class DbException : public exception {
   public:
    // Pass by value and move instead of pass by const&:
    // https://stackoverflow.com/questions/51705967/advantages-of-pass-by-value-and-stdmove-over-pass-by-reference/51706522
    // https://stackoverflow.com/questions/26261007/why-is-value-taking-setter-member-functions-not-recommended-in-herb-sutters-cpp
    explicit DbException(string msg) : m_msg(move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return m_msg.c_str();
    }

   private:
    const string m_msg;
};
}  // namespace projectdb

#endif  // MAIN_EXCEPTION_H

//
// Created by Mengwen Li (ml4643) on 3/20/2021.
//

#ifndef MAIN_TRANSACTION_LOG_H
#define MAIN_TRANSACTION_LOG_H

#include "key.h"
#include "value.h"

namespace projectdb {

enum class DbTransactionType { GET, SET, REMOVE };

// template <DbTransactionType T>
// class Transaction {
//    static_assert(sizeof(T) == -1, "Transaction type not supported for
//    TransactionLog!");
//};
//
// template <>
// class Transaction<DbTransactionType::SET> {
// public:
//    Transaction()
//};

}  // namespace projectdb

#endif  // MAIN_TRANSACTION_LOG_H

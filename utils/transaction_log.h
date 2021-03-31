//
// Created by Mengwen Li (ml4643) on 3/20/2021.
//

#ifndef MAIN_TRANSACTION_LOG_H
#define MAIN_TRANSACTION_LOG_H

#include <fstream>

#include "db_concepts.h"
#include "key.h"
#include "serializer.h"
#include "value.h"

namespace projectdb {

enum class DbTransactionType { GET, SET, REMOVE };

class MemTable;

class TransactionLog {
   public:
    TransactionLog();

    template <Serializable... Ts>
    void logTransaction(DbTransactionType type, const Ts&... args) {
        SerializationWrapper<DbTransactionType>{type}(m_fs);
        logTransactionImpl(args...);
    }

    void populateMemTable(MemTable* memTable);

   private:
    template <Serializable T>
    void logTransactionImpl(const T& arg) {
        SerializationWrapper<T>{arg}(m_fs);
    }
    template <Serializable T, Serializable... Ts>
    void logTransactionImpl(const T& arg, const Ts&... args) {
        SerializationWrapper<T>{arg}(m_fs);
        logTransactionImpl(args...);
    }

    static string genFileName();

    fstream m_fs;
};

}  // namespace projectdb

#endif  // MAIN_TRANSACTION_LOG_H

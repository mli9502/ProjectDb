//
// Created by Mengwen Li (ml4643) on 3/20/2021.
//

#ifndef MAIN_TRANSACTION_LOG_H
#define MAIN_TRANSACTION_LOG_H

#include <fstream>

#include "db_concepts.h"
#include "memtable.h"
#include "serializer.h"

namespace projectdb {

enum class DbTransactionType { GET, SET, REMOVE };

class TransactionLogWritter {
   public:
    TransactionLogWritter();
    TransactionLogWritter(string transactionLogFileName);

    string getTransactionLogFileName() const;

    template <Serializable... Ts>
    void write(DbTransactionType type, const Ts&... args) & {
        SerializationWrapper<DbTransactionType>{type}(m_ofs);
        writeImpl(args...);
        // NOTE: @mli: We need to flush here to make sure that records is
        // flushed to disk right away, instead of needing to wait for m_ofs to
        // be destructed.
        m_ofs.flush();
    }

   private:
    template <Serializable T>
    void writeImpl(const T& arg) {
        SerializationWrapper<T>{arg}(m_ofs);
    }
    template <Serializable T, Serializable... Ts>
    void writeImpl(const T& arg, const Ts&... args) {
        SerializationWrapper<T>{arg}(m_ofs);
        writeImpl(args...);
    }

    string m_transactionLogFileName;
    fstream m_ofs;
};

class TransactionLogLoader {
   public:
    static MemTable load(string_view transactionLogFileName);
};

}  // namespace projectdb

#endif  // MAIN_TRANSACTION_LOG_H

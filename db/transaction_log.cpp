//
// Created by Mengwen Li (ml4643) on 3/20/2021.
//

#include "transaction_log.h"

#include "system_utils.h"

namespace projectdb {

TransactionLogWritter::TransactionLogWritter()
    : m_transactionLogFileName(genTransactionLogFileName()),
      m_ofs(getFileStream(m_transactionLogFileName, ios_base::app)) {}

TransactionLogWritter::TransactionLogWritter(string transactionLogFileName)
    : m_transactionLogFileName(move(transactionLogFileName)),
      m_ofs(getFileStream(m_transactionLogFileName, ios_base::app)) {}

string TransactionLogWritter::getTransactionLogFileName() const {
    return m_transactionLogFileName;
}

MemTable TransactionLogLoader::load(string_view transactionLogFileName) {
    MemTable rtn;
    auto ifs = getFileStream(transactionLogFileName, ios_base::in);
    while (true) {
        DbTransactionType transactionType;
        try {
            transactionType = DeserializationWrapper<DbTransactionType>{}(ifs);
            log::debug("Found type: ",
                       static_cast<underlying_type_t<DbTransactionType>>(
                           transactionType));
            switch (transactionType) {
                case DbTransactionType::SET: {
                    const auto& key =
                        DeserializationWrapper<Table::key_type>{}(ifs);
                    const auto& value =
                        DeserializationWrapper<Table::mapped_type>{}(ifs);
                    // NOTE: @mli:
                    // We can't just use DeserializationWrapper{}()
                    // as the two arguments, because the evaluation order of
                    // function arguments is not determined.
                    // https://en.cppreference.com/w/cpp/language/eval_order
                    rtn.set(key, value);
                    break;
                }
                case DbTransactionType::REMOVE: {
                    const auto& key =
                        DeserializationWrapper<Table::key_type>{}(ifs);
                    rtn.remove(key);
                    break;
                }
                default: {
                    log::errorAndThrow(
                        "Found unsupported transaction type: ",
                        static_cast<underlying_type_t<DbTransactionType>>(
                            transactionType),
                        " while parsing transaction log file: ",
                        transactionLogFileName);
                }
            }
        } catch (const DbException& e) {
            if (ifs.eof()) {
                log::debug("Reached end of transaction log file: ",
                           transactionLogFileName);
                break;
            } else {
                log::error("Exception while reading transaction log file: ",
                           transactionLogFileName);
                throw;
            }
        }
    }
    return rtn;
}

}  // namespace projectdb

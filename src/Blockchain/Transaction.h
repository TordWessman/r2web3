#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <vector>
#include <stdint.h>

namespace blockchain
{
    /// @brief Interface for a blockchain transaction.
    class Transaction
    {
    public:
        /// @brief Serialize a complete `Transaction`
        /// @return
        virtual std::vector<uint8_t> Serialize() const
    #ifndef ARDUINO
            {
                throw std::runtime_error("Transaction::Serialize() must be overloaded");
            }
    #else
                = 0;
    #endif
    };
}
#endif
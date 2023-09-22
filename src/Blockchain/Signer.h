#ifndef __SIGNER_H__
#define __SIGNER_H__

#include <vector>
#include <stdint.h>
#include "Transaction.h"

namespace blockchain
{
        #define KECCAK_256_LENGTH 32

        /// @brief Generates a Keccek256 hash.
        /// @param digest the hashed data.
        /// @param length the size of the hash to return. Defaults to 32 bytes.
        /// @return 
        std::vector<uint8_t> Keccak256(const std::vector<uint8_t> *digest, const size_t length = KECCAK_256_LENGTH);

        /// @brief Interface for signing transactions.
        /// @tparam TransactionType is the type of transaction objects this signer is capable to sign.
        template <typename TransactionType>
        class Signer
        {
        public:
                /// @brief Signs the transaction using `privateKey`
                /// @param transaction 
                /// @param privateKey 
                /// @return a new, signed transaction.
                virtual TransactionType Sign(const TransactionType *transaction, const std::vector<uint8_t> *privateKey) const
        #ifndef ARDUINO
                { throw std::runtime_error("Signer::Sign(Transaction transaction) must be overloaded"); }
        #else
                = 0;
        #endif

                virtual Signer *clone() const
        #ifndef ARDUINO
                { throw std::runtime_error("Signer::clone() must be overloaded"); }
        #else
                = 0;
        #endif

        };
}
#endif
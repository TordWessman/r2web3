/**
 * MIT License
 *
 * Copyright (c) 2023 Tord Wessman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
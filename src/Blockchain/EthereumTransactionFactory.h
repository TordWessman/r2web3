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

#ifndef __ETHEREUM_TRANSACTION_FACTORY_H__
#define __ETHEREUM_TRANSACTION_FACTORY_H__

#include <vector>

#include "../Shared/Common.h"
#include "Account.h"
#include "EthereumSigner.h"
#include "Signer.h"
#include "EthereumTransaction.h"
#include "TransactionFactory.h"

namespace blockchain
{
    /// @brief Default `TransactionFactory` implementation, responsible for signing ethereum-compatible transactions.
    class EthereumTransactionFactory : TransactionFactory<EthereumTransactionProperties, EthereumTransaction>
    {
    public:
        EthereumTransactionFactory() : signer(new EthereumSigner()) {}

        ~EthereumTransactionFactory()
        {
            delete signer;
        }

        /// @brief Creates and signs a transaction.
        /// @param properties Transaction information.
        /// @param privateKey Key used to sign the transaction.
        /// @return A signed `EthereumTransaction`
        EthereumTransaction GenerateTransaction(const EthereumTransactionProperties &properties, std::vector<uint8_t> *privateKey) const override
        {
            EthereumTransaction t_unsigned(properties);

            EthereumTransaction t_signed = signer->Sign(&t_unsigned, privateKey);

            return t_signed;
        }

        /// @brief Uses the account's private key to create and sign an `EthereumTransaction`.
        /// @param properties The transaction data to be signed and serialied.
        /// @param account An account containing the signing key
        /// @return the signed transaction as a hex string. This result must be manually deallocated after usage.
        char *GenerateSerializedData(const EthereumTransactionProperties properties, const Account *account) const override
        {
            std::vector<uint8_t> pk = account->GetPrivateKey();

            EthereumTransaction tx = GenerateTransaction(properties, &pk);

            std::vector<uint8_t> serializedTransaction = tx.Serialize();

            char *parameter = (serializedTransaction | byte_array::hex_string) | char_string::add_hex_prefix;

            return parameter;
        }

        EthereumTransactionFactory &operator=(const EthereumTransactionFactory &other)
        {
            if (this != &other)
            {
                delete signer;
                signer = other.signer->clone();
            }
            return *this;
        }

    private:
        const Signer<EthereumTransaction> *signer;
    };
}
#endif
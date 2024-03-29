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

#ifndef __ETHEREUM_TRANSACTION_H__
#define __ETHEREUM_TRANSACTION_H__

#include <vector>
#include <stdint.h>

#include "../Shared/BigNumber.h"
#include "../Shared/Common.h"
#include "Transaction.h"
#include "Encodable.h"
#include "Signer.h"
#include "Transaction.h"
#include "RlpEncoder.h"
namespace blockchain
{
    /// @brief A ecdsa signature for a transaction split into it's sub-component
    struct EthereumSignature
    {
        /// @brief Creates an "unsigned signature" using chainId for the signature's `v` parameter.
        /// @param v
        EthereumSignature(uint32_t chainId) : v(chainId) {}

        /// @brief Create a signed signature using a calculated `v` and the ESCDA signature.
        /// @param v
        /// @param signature
        EthereumSignature(uint32_t v, const std::vector<uint8_t> signature)
            : v(v),
              r(std::vector<uint8_t>(signature.begin(), signature.begin() + signature.size() / 2) | byte_array::truncate),
              s(std::vector<uint8_t>(signature.begin() + signature.size() / 2, signature.end()) | byte_array::truncate)

        {}

        EthereumSignature(uint32_t v, const std::vector<uint8_t> r, const std::vector<uint8_t> s) : v(v), r(r), s(s) {}

        const std::vector<uint8_t> r;
        const std::vector<uint8_t> s;
        const uint32_t v;
    };

    /// @brief Signing standard being used for transactions.
    enum class EthereumSigningStandard
    {
        Legacy,
        EIP1559 // also EIP-2930
    };

    /// @brief Represents the properties required for a transaction for Ethereum compatible chains.
    struct EthereumTransactionProperties
    {
        EthereumTransactionProperties(uint32_t nonce, BigNumber gasPrice, uint32_t gasLimit,
                                      Address address, BigNumber value, std::vector<uint8_t> data,
                                      uint32_t chainId) : nonce(nonce), gasPrice(gasPrice), gasLimit(gasLimit),
                                                        address(address), value(value), data(data),
                                                        chainId(chainId), signingStandard(EthereumSigningStandard::Legacy)
        {
            
        }

        const EthereumSigningStandard signingStandard;
        const uint32_t nonce;
        const BigNumber gasPrice;
        const uint32_t gasLimit;
        const Address address;
        const BigNumber value;
        std::vector<uint8_t> data;
        const uint32_t chainId;
    };

    class EthereumTransaction : public Transaction
    {
    public:
        /// @brief Cunstruct a transaction without a signature.
        EthereumTransaction(EthereumTransactionProperties properties) : properties(properties), 
                                                                        signature(EthereumSignature(properties.chainId)),
                                                                        encoder(new RlpEncoder()) {}
        /// @brief Cunstruct a transaction including a signature.
        EthereumTransaction(EthereumTransactionProperties properties, EthereumSignature signature) : properties(properties),
                                                                                                        signature(signature),
                                                                                                        encoder(new RlpEncoder()) {}

        EthereumTransaction(const EthereumTransaction& other) : properties(other.properties), 
                                                                signature(other.signature), 
                                                                encoder(other.encoder->clone()) {}

        ~EthereumTransaction()
        {
            delete encoder;
        }

        EthereumTransaction &operator=(const EthereumTransaction &other)
        {
            if (this != &other)
            {
                delete encoder;
                encoder = other.encoder->clone();
            }
            return *this;
        }

        /// @brief Serializes the transaction using the provided `encoder`.
        /// @return 
        std::vector<uint8_t> Serialize() const override;

        EthereumTransactionProperties Properties() const { return properties; }
        //void SetEncoder(Encoder encoder) : encoder(encoder) {}

    private:
        const EthereumTransactionProperties properties;
        const EthereumSignature signature;
        EncoderX *encoder;

    };
}
#endif
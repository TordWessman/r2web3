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

#ifndef __ETHEREUM_SIGNER_H__
#define __ETHEREUM_SIGNER_H__
#include <vector>
#include <stdint.h>

#include "Transaction.h"
#include "Signer.h"
#include "EthereumTransaction.h"

namespace blockchain
{
    #define ETHEREUM_SIGNATURE_LENGTH 64
    #define KECCAK_256_LENGTH 32

    /// @brief Signing of EVM-compatible data.
    class EthereumSigner: public Signer<EthereumTransaction>
    {
    public:

        EthereumSigner() {}

        /// @brief Sign an `EthereumTransaction` using a private key and return the signed `EthereumTransaction`.    
        EthereumTransaction Sign(const EthereumTransaction *transaction, const std::vector<uint8_t> *privateKey) const override;

        EthereumSigner *clone() const override { return new EthereumSigner(*this); }

    private:
        /// @brief Sign any message using the private `key`.
        static EthereumSignature GenerateSignature(const std::vector<uint8_t> *message, const std::vector<uint8_t> *key);
    };
}
#endif

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

#include "EthereumSigner.h"

#include "../cryptography/sha3.h"
#include "../cryptography/secp256k1.h"
#include "../cryptography/ecdsa.h"

namespace blockchain
{
    EthereumSignature EthereumSigner::GenerateSignature(const std::vector<uint8_t> *message, const std::vector<uint8_t> *key)
    {
        uint8_t result[ETHEREUM_SIGNATURE_LENGTH];
        uint8_t digest[message->size()];
        uint8_t privateKey[key->size()];
        
        for(size_t i = 0; i < message->size(); i++) { digest[i] = (*message)[i]; }
        for(size_t i = 0; i < key->size(); i++) { privateKey[i] = (*key)[i]; }

        const ecdsa_curve *curve = &secp256k1;
        uint8_t pby;
        int res = 0;
        bool allZero = true;
        res = ecdsa_sign_digest(curve, privateKey, digest, result, &pby, NULL);
        std::vector<uint8_t> signature;
        for (int i = 0; i < ETHEREUM_SIGNATURE_LENGTH; i++)
        {
            signature.push_back(result[i]);
        }
        return EthereumSignature(pby, signature);
    }

    EthereumTransaction EthereumSigner::Sign(const EthereumTransaction *transaction, const std::vector<uint8_t> *privateKey) const
    {
        std::vector<uint8_t> tx = transaction->Serialize();
        if (transaction->Properties().signingStandard == EthereumSigningStandard::EIP1559)
        {
            tx.insert(tx.begin(), {0x2});
        }

        std::vector<uint8_t> hash = Keccak256(&tx);

        EthereumSignature signatureOriginal = EthereumSigner::GenerateSignature(&hash, privateKey);

        uint32_t v = signatureOriginal.v;

        if (transaction->Properties().signingStandard == EthereumSigningStandard::Legacy)
        {
            v = v + transaction->Properties().chainId * 2 + 0x23;
        }

        EthereumSignature signature = EthereumSignature(v, signatureOriginal.r, signatureOriginal.s);

        return EthereumTransaction(transaction->Properties(), signature);
    }
}
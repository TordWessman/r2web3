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

        //Log::m("r: ", signatureOriginal.r);
        //Log::m("s: ", signatureOriginal.v);
        Log::m("v: ", v);
        EthereumSignature signature = EthereumSignature(v, signatureOriginal.r, signatureOriginal.s);

        return EthereumTransaction(transaction->Properties(), signature);
    }
}
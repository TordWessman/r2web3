#include "Account.h"

#include "../cryptography/sha3.h"
#include "../cryptography/secp256k1.h"
#include "../cryptography/ecdsa.h"

#include "../Shared/BigNumber.h"

namespace blockchain
{
    std::vector<uint8_t> Account::CalculatePublicKey(const std::vector<uint8_t> *privateKey, size_t publicKeySize)
    {
        uint8_t publicKeyBuffer[publicKeySize + 1];
        uint8_t privateKeyBuffer[privateKey->size()];
        std::vector<uint8_t> publicKey;

        for (size_t i = 0; i < privateKey->size(); i++) { privateKeyBuffer[i] = (*privateKey)[i]; }

        const ecdsa_curve *curve = &secp256k1;
        ecdsa_get_public_key65(curve, privateKeyBuffer, publicKeyBuffer);

        for (size_t i = 1; i < publicKeySize + 1; i++) { publicKey.push_back(publicKeyBuffer[i]); }

        return publicKey;
    }

    std::vector<uint8_t> Account::CalculateAddress(const std::vector<uint8_t> *publicKey)
    {
        uint8_t publicKeyBuffer[publicKey->size()];
        uint8_t publicKeyHash[32];
        std::vector<uint8_t> address;

        for (size_t i = 0; i < publicKey->size(); i++) { publicKeyBuffer[i] = (*publicKey)[i]; }
        keccak_256(publicKeyBuffer, publicKey->size(), publicKeyHash);

        for(size_t i = 0; i < 20; i++) { address.push_back(publicKeyHash[i + 12]); }

        return address;
    }
}
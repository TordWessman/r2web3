#include "Signer.h"

#include "../cryptography/sha3.h"
#include "../cryptography/secp256k1.h"
namespace blockchain
{
    std::vector<uint8_t> Keccak256(const std::vector<uint8_t> *digest, const size_t length)
    {
        uint8_t input[digest->size()];
        uint8_t output[length];

        for (int i = 0; i < digest->size(); i++)
        {
            input[i] = digest->at(i);
        }
        keccak_256(input, digest->size(), output);
        std::vector<uint8_t> hash(length);
        for (int i = 0; i < length; i++)
        {
            hash[i] = output[i];
        }
        return hash;
    }
}
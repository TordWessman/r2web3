#ifndef __CHAIN_ACCOUNT_H__
#define __CHAIN_ACCOUNT_H__

#include <vector>
#include <stdint.h>

#include "../Shared/Common.h"
#include "Address.h"

#define ETH_PUBLIC_KEY_SIZE 64

namespace blockchain
{
    /// @brief Represents a user-controlled "account"
    class Account
    {
    public:

        /// @brief Instantiate using a private key. The account address and the public key will be derived from this.
        /// @param pk private key.
        Account(const char *pk) : privateKey(pk | byte_array::hex_string_to_bytes), address(Account::GenerateAddress(&privateKey)) {}

        static Address GenerateAddress(std::vector<uint8_t> *privateKey) 
        {
            std::vector<uint8_t> publicKey = CalculatePublicKey(privateKey);
            char *hexString = (CalculateAddress(&publicKey) | byte_array::hex_string);
            Address address = Address(hexString);
            delete[] hexString;
            return address;
        }

        /// @brief Returns the calculated address.
        /// @return 
        Address GetAddress() const { return address; }

        /// @brief Return the private key associated with this account.
        /// @return 
        std::vector<uint8_t> GetPrivateKey() const { return privateKey; }

        static std::vector<uint8_t> CalculateAddress(const std::vector<uint8_t> *publicKey);

        /// @brief Calculate a public key using a private key 
        /// @param privateKey 
        /// @param publicKeySize The returned public key size. Defaults to 64.
        /// @return 
        static std::vector<uint8_t> CalculatePublicKey(const std::vector<uint8_t> *privateKey, size_t publicKeySize = ETH_PUBLIC_KEY_SIZE);

    private:
        std::vector<uint8_t> privateKey;
        Address address;
    };
}
#endif
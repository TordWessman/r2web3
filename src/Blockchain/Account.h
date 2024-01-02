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
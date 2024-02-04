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

#ifndef __ADDRESS__H__
#define __ADDRESS__H__

#include <vector>
#include <cstring>
#include <cassert>

namespace blockchain
{       
    #define ETH_ADDRESS_LENGTH 42

    /// @brief Represents a blockchain address.
    class Address
    {
    public:

        /// @brief Instantiate an empty address.
        Address() {
            memset(address, '0', sizeof(address));
            address[0] = '0';
            address[1] = 'x';
            address[sizeof(address)] = '\0';
        }

        /// @brief Create an `Address` using the last `ETH_ADDRESS_LENGTH` bytes.
        /// @param bytes
        Address(const std::vector<uint8_t> *bytes)
        {
            Initialize(bytes);
        }

        /// @brief Create an `Address` using the last `ETH_ADDRESS_LENGTH` bytes.
        /// @param bytes
        Address(const std::vector<uint8_t> bytes)
        {
            Initialize(&bytes);
        }


        /// @brief Create an `Address` using its hex-representation (with or without 0x prefix).
        /// @param addressString 
        Address(const char *addressString)
        {
            if (addressString == NULL)
            {
                memset(address, '0', sizeof(address));
                address[0] = '0';
                address[1] = 'x';
                return;
            }

            assert(strlen(addressString) == ETH_ADDRESS_LENGTH || strlen(addressString) == ETH_ADDRESS_LENGTH - 2);

            if (addressString[0] == '0' && (addressString[1] == 'x' || addressString[1] == 'X'))
            {
                for (size_t i = 0; i < ETH_ADDRESS_LENGTH; i++) { address[i] = addressString[i]; }
            }
            else
            {
                for (size_t i = 0; i < ETH_ADDRESS_LENGTH - 2; i++) { address[i + 2] = addressString[i]; }
            }
            address[0] = '0'; address[1] = 'x';
            address[ETH_ADDRESS_LENGTH] = '\0';
        }

        // Return the (human readable) address.
        const char* AsString() const { return address; }

        Address *clone() const { return new Address(*this); }

    private:
        
        char address[ETH_ADDRESS_LENGTH + 1];

        void Initialize(const std::vector<uint8_t> *bytes)
        {
            const size_t len = (ETH_ADDRESS_LENGTH - 2) / 2;
            assert(bytes->size() >=  len);
            for(size_t i = 0; i < bytes->size(); i++)
            {
                address[2 + i] = bytes->at(i);
            }
            address[ETH_ADDRESS_LENGTH] = '\0';
            address[0] = '0'; address[1] = 'x';
        }
    };
}
#endif
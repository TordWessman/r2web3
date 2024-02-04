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

#ifndef __BIG_NUMBER_H__
#define __BIG_NUMBER_H__

#include <vector>
#include <stdint.h>
#include <cstring>

namespace blockchain
{
    /// @brief Represents a "large" real number.
    class BigNumber
    {
        enum Sign {
            Positive,
            Negative
        };

    public:
        /// @brief Represents '0'
        BigNumber();

        /// @brief Constructor using a hexadecimal string.
        /// @param value A hex-string with - or without the "0x" prefix.
        BigNumber(const char *hexString);

        /// @brief Constructor using an unsigned 32-bit value.
        /// @param value
        BigNumber(uint32_t value);

        /// @brief Construct using an array of 16-bit values.
        /// @param value
        BigNumber(const std::vector<uint16_t> value);

        /// @brief A floating point representation of a value, which will transform it to it's gwei-representation
        /// @param toGwei The value in whole units.
        /// @param decimals The number of decimals to use.
        BigNumber(float toGwei, uint8_t decimals);

        /// @brief Copy reference.
        /// @param other 
        BigNumber(const BigNumber *other);

        BigNumber(const BigNumber &other)
        {
            if (other.hexString != nullptr)
            {
                hexString = new char[strlen(other.hexString) + 1];
                memcpy(hexString, other.hexString, strlen(other.hexString));
                hexString[strlen(other.hexString)] = '\0';
            }
            data = other.data;
        }

        BigNumber &operator=(const BigNumber &other)
        {
            if (this != &other)
            {
                if (other.hexString != nullptr)
                {
                    delete[] hexString;
                    hexString = new char[strlen(other.hexString) + 1];
                    memcpy(hexString, other.hexString, strlen(other.hexString) + 1);
                }
                data = other.data;
            }
            return *this;
        }

        ~BigNumber()
        {
            if (hexString != nullptr)
            {
                delete[] hexString;
            }
            hexString = nullptr;
        }

        /// @brief Return the hexadecimal representation of the value.
        /// @return
        char *HexString() const { return hexString; }

        /// @brief Return the raw representation of the value.
        /// @return
        std::vector<uint8_t> Bytes() const;

        /// @brief Generates an 32-bit integer value. Will throw if the size contained value is > 4 bytes.
        /// @return
        uint32_t ToUInt32() const;

        /// @brief Will generate an array with a decimal representation of the number. The return value must manually be deallocated
        /// @return
        char *GenerateDecimalString() const;

        BigNumber *clone() const { return new BigNumber(*this); }

        /// @brief Śign of the number. Currently, only positive numbers are supported.
        const Sign Sign = Sign::Positive;

    private:
        std::vector<uint16_t> data;
        char *hexString;
        void GenerateHexString();
    };
}
#endif
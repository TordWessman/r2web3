#include <sstream>
#include <iomanip>

#include "BigNumber.h"
#include "../Shared/Common.h"

namespace blockchain
{
    char *intToChar(int number)
    {
        int temp = number;
        int length = 1;
        while (temp /= 10)
        {
            length++;
        }

        char *str = new char[length + 1];
        temp = number;
        str[length] = '\0';
        for (int i = length - 1; i >= 0; i--)
        {
            str[i] = '0' + (temp % 10);
            temp /= 10;
        }
        return str;
    }

    /// @brief remove any leading '0' characters. Warning! This method will free(str).
    /// @param str
    /// @return
    char *removeLeadingZeros(char *str)
    {
        int i = 0;
        while (str[i] == '0')
        {
            i++;
        }

        if (i == 0)
        {
            return str;
        }

        size_t result_size = strlen(str) - i + 1;

        char *result = new char[result_size];
        memcpy(result, str + i, result_size);
        free(str);
        return result;
    }

    /// @brief Treat the strings as numbers and add them. Return an allocated char * containing the sum. Result must be `free`d in order to avoid memory leaks.
    /// @param lhs
    /// @param rhs
    /// @return
    char *addStrings(const char *lhs, const char *rhs)
    {
        size_t lhsLength = strlen(lhs), rhsLength = strlen(rhs);
        size_t length = std::max(lhsLength, rhsLength) + 1;
        uint16_t result[length] = {};

        for (size_t i = 0; i < length; i++)
        {
            if (i < lhsLength)
            {
                result[i] += lhs[lhsLength - i - 1] - '0';
            }
            if (i < rhsLength)
            {
                result[i] += rhs[rhsLength - i - 1] - '0';
            }
        }

        char *resultString = (char *)malloc(length + 1);
        resultString[length] = '\0';

        for (size_t i = 0; i < length - 1; i++)
        {
            result[i + 1] += result[i] / 10;
            result[i] = result[i] % 10;
        }

        for (size_t i = 0; i < length; i++)
        {
            resultString[i] = result[length - i - 1] + '0';
        }

        return removeLeadingZeros(resultString);
    }

    /// @brief Treat the strings as numbers and multipy them. Return an allocated char * containing the product. Result must be `free`d in order to avoid memory leaks.
    /// @param lhs
    /// @param rhs
    /// @return
    char *multiplyStrings(const char *lhs, const char *rhs)
    {
        size_t alength = strlen(lhs);
        size_t blength = strlen(rhs);
        size_t length = alength + blength + 1;

        uint result[length] = {};

        uint16_t a, b, i, val, digit;
        for (size_t ia = 0; ia < alength; ia++)
        {
            a = lhs[alength - ia - 1] - '0';
            for (size_t ib = 0; ib < blength; ib++)
            {
                b = rhs[blength - ib - 1] - '0';
                i = ia + ib;
                val = a * b;
                digit = val % 10;
                result[i] += digit;
                result[i + 1] += val / 10;
            }
        }

        char *resultString = (char *)malloc(length + 1);
        resultString[length] = '\0';

        for (size_t i = 0; i < length - 1; i++)
        {
            result[i + 1] += result[i] / 10;
            result[i] = result[i] % 10;
        }

        for (size_t i = 0; i < length; i++)
        {
            resultString[i] = result[length - i - 1] + '0';
        }

        return removeLeadingZeros(resultString);
    }

    BigNumber::BigNumber()
    {
        hexString = new char[1];
        hexString[0] = '\n';
    }

    BigNumber::BigNumber(uint32_t value)
    {
        data.insert(data.begin(), value & 0xFFFF);
        if (value > 0xFFFF)
        {
            data.insert(data.begin(), (value >> 16));
        }
        GenerateHexString();
    }

    BigNumber::BigNumber(const std::vector<uint16_t> value)
    {
        data = value;
        GenerateHexString();
    }

    BigNumber::BigNumber(const char *hexString)
    {
        size_t length = strlen(hexString);
        string_info str(hexString, length);
        string_info hexLetters = (str | char_string::remove_hex_prefix);
        uint16_t byteValue;
        size_t dataTypeLength = sizeof(byteValue) * 2;

        size_t i = hexLetters.length;
        size_t len;
        char tmp[dataTypeLength + 1];
        while (i > 0)
        {
            memset(tmp, 0, dataTypeLength + 1);
            len = (size_t)std::min(i, dataTypeLength);
            memcpy(tmp, hexLetters.value + hexLetters.begin + i - len, len);
            byteValue = strtol(tmp, nullptr, 16);
            data.insert(data.begin(), byteValue);
            if (i - len < 0)
            {
                i = 0;
            }
            else
            {
                i -= len;
            }
        }
        GenerateHexString();
    }

    BigNumber::BigNumber(float toGwei, uint8_t decimals = 18)
    {
        THROW("Not implemented yet.");
        GenerateHexString();
    }

    BigNumber::BigNumber(const BigNumber *other)
    {
        if (other != nullptr)
        {
            for (uint16_t part: other->data) 
            {
                data.push_back(part);
            }
        }
        GenerateHexString();
    }

    void BigNumber::GenerateHexString()
    {
        const char *hexStringFormat = "%04X";
        size_t length = 2 * data.size() * sizeof(uint16_t) + 1;
        hexString = new char[length];
        hexString[length - 1] = '\0';
        for (int i = 0; i < data.size(); i++)
        {
            std::snprintf(hexString + i * 2 * sizeof(uint16_t), 2 * sizeof(uint16_t) + 1, hexStringFormat, data[i]);
        }
    }

    char *BigNumber::GenerateDecimalString() const
    {
        char *decimalString = new char[2];
        decimalString[0] = '0';
        decimalString[1] = '\0';
        if (data.size() == 0)
        {
            return decimalString;
        }

        const char *factor = "65536";

        char *tmpNumber;
        char *tmpProduct;

        for (size_t i = 0; i < data.size(); i++)
        {
            tmpNumber = intToChar((int)data[i]);

            for (size_t j = 0; j < data.size() - i - 1; j++)
            {
                tmpProduct = multiplyStrings(tmpNumber, factor);

                delete[] tmpNumber;
                tmpNumber = new char[strlen(tmpProduct) + 1];
                memcpy(tmpNumber, tmpProduct, strlen(tmpProduct) + 1);
                delete[] tmpProduct;
            }

            tmpProduct = addStrings(decimalString, tmpNumber);
            delete[] tmpNumber;
            delete[] decimalString;
            decimalString = new char[strlen(tmpProduct) + 1];
            memcpy(decimalString, tmpProduct, strlen(tmpProduct) + 1);
            delete[] tmpProduct;
        }
        return decimalString;
    }

    std::vector<uint8_t> BigNumber::Bytes() const
    {
        std::vector<uint8_t> result;

        const size_t epe = sizeof(uint16_t) / sizeof(uint8_t);
        uint16_t value;
        size_t digitCount;
        for (int i = 0; i < data.size(); i++)
        {
            value = data[i];
            digitCount = 0;
            std::vector<uint8_t> word;
            for (size_t j = 0; j < epe; j++)
            {
                word.insert(word.begin(), static_cast<uint8_t>(value & 0xFF));
                value >>= 8;
            }
            for (uint8_t digit : word)
            {
                result.push_back(digit);
            }
        }
        return result;
    }

    uint32_t BigNumber::ToUInt32() const
    {
        if (data.size() > 2)
        {
            THROW("Can't convert decimal string to int. Number is too large.");
        }
        if (data.size() == 0)
        {
            return 0;
        }
        char *decimalString = GenerateDecimalString();
        uint32_t value = strtol(decimalString, nullptr, 10);
        delete[] decimalString;
        return value;
    }
}
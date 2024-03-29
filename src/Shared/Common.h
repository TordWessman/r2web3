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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <cstring>
#include <algorithm>
#include <vector>
#include <stdint.h>

#ifdef ARDUINO
    void _throwException_arduino(const char *message);
    #define THROW(message) _throwException_arduino(message)
#else
    #include <stdexcept>
    #define THROW(message) throw std::runtime_error(message)
#endif

namespace blockchain
{

    /// @brief Object used internally for string manipulations where a reference to the array is being stored along with some meta-properties.
    struct string_info
    {
        string_info(const string_info &info) : value(info.value), begin(info.begin), length(info.length) {}
        string_info(const char *str) : value(str), begin(0), length(strlen(str)) {}
        string_info(const char *str, size_t length) : value(str), begin(0), length(length) {}
        string_info(const char *str, size_t begin, size_t length) : value(str), begin(begin), length(length) {}

        /// @brief The perceived start of the string.
        const size_t begin;

        /// @brief the length of the string measured from `start`
        const size_t length;

        /// @brief a reference to the string.
        const char *value;
    };

    /// @brief extensions for `vector<uint8_t>`.
    namespace byte_array
    {
        struct truncate_t
        {
            std::vector<uint8_t> operator()(const std::vector<uint8_t> &v) const;
        };
        struct hex_string_t
        {
            char *operator()(const std::vector<uint8_t> &v) const;
        };
        struct to_string_t
        {
            char *operator()(const std::vector<uint8_t> &v) const;
        };
        struct hex_string_to_bytes_t
        {
            std::vector<uint8_t> operator()(const char *v) const;
        };
        struct string_to_bytes_t
        {
            std::vector<uint8_t> operator()(const char *v) const;
        };
        struct uint_to_bytes_t
        {
            std::vector<uint8_t> operator()(const uint32_t v) const;
        };
        struct size_to_bytes_t
        {
            std::vector<uint8_t> operator()(const size_t v) const;
        };

        /// @brief Remove leading 0's from the array
        const truncate_t truncate = {};
        /// @brief Return the vector as a hex-string. Please note that the returned string needs to be deallocated manually.
        const hex_string_t hex_string = {};
        /// @brief Return the vector as a char* representation. Please note that the returned string needs to be deallocated manually.
        const to_string_t to_string = {};
        /// @brief Use a hex-string has input and return the corresponding byte array.
        const hex_string_to_bytes_t hex_string_to_bytes = {};
        /// @brief Use a any string has input and return the corresponding byte array.
        const string_to_bytes_t string_to_bytes = {};
        /// @brief Convert unsigned 32-bit int to big-endian byte array.
        const uint_to_bytes_t uint_to_bytes = {};
        /// @brief Convert size_t to big-endian byte array.
        const size_to_bytes_t size_to_bytes = {};

        std::vector<uint8_t> operator|(const std::vector<uint8_t> &v, truncate_t f);
        char *operator|(const std::vector<uint8_t> &v, hex_string_t f);
        char *operator|(const std::vector<uint8_t> &v, to_string_t f);
        std::vector<uint8_t> operator|(const char *v, hex_string_to_bytes_t f);
        std::vector<uint8_t> operator|(const char *v, string_to_bytes_t f);
        std::vector<uint8_t> operator|(const uint32_t v, uint_to_bytes_t f);
        std::vector<uint8_t> operator|(const size_t v, size_to_bytes_t f);
    }

    namespace char_string
    {
        struct add_hex_prefix_t
        {
            char *operator()(char *v);
        };
        struct ltrim_t
        {
            string_info operator()(const string_info &v);
        };
        struct remove_hex_prefix_t
        {
            string_info operator()(const string_info &v);
        };
        struct is_null_t
        {
            bool operator()(char *v);
        };
        struct retain_t
        {
            char *operator()(char *v);
        };
        struct copy_t
        {
            char *operator()(const char *v);
        };

        /// @brief __DEALLOCATES__ input and returns a new string with the "0x" prefix.
        /// Please note that the returned string needs to be deallocated manually.
        const add_hex_prefix_t add_hex_prefix = {};
        /// @brief returns a `string_info` where the `length` and `begin` is adjusted to the string omitting leading white-spaces.
        const ltrim_t ltrim = {};
        /// @brief returns a `string_info` struct which where the `length` and `begin` is adjusted
        /// to the trimmed string where the hex-prefix ("0x") is omitted (if present).
        const remove_hex_prefix_t remove_hex_prefix = {};
        /// @brief Returns true if string is NULL or "null".
        const is_null_t is_null = {};
        /// @brief Creates a retained (allocated) version of the char *
        const retain_t retain = {};
        /// @brief Creates a retained (allocated) version of the const char *
        const copy_t copy = {};

        char *operator|(char *v, add_hex_prefix_t f);
        string_info operator|(const string_info &v, ltrim_t f);
        string_info operator|(const string_info &v, remove_hex_prefix_t f);
        bool operator|(char *v, is_null_t f);
        char *operator|(char *v, retain_t f);
        char *operator|(const char *v, copy_t f);
    }

    /// @brief Implementations can contain error information.
    class ErrorDescription
    {

    public:
        /// @brief Returns the provided error-code or `0` if no error code was provided.
        /// @return
        virtual int ErrorCode() const = 0;

        /// @brief Returns an error message if provided.
        /// @return
        virtual const char *ErrorMessage() const = 0;

    };

    /// @brief A simple enumeration for wrapping an "optional" value as a "success" or "failure".
    /// The value contained is not memory managed and might have to be manually deallocated.
    template <typename T>
    class Result : public ErrorDescription
    {
    public:
        /// @brief Creates a "successful" result.
        /// @param value
        Result(T value) : hasValue(true), value(value), errorCode(0), errorMessage(nullptr) { }

        /// @brief Generate a failure with an error code.
        /// @param errorCode
        /// @return
        static Result<T> Err(int errorCode) { return Result<T>(errorCode, false); }

        /// @brief Generate a "failure" with an error code and a descriptive message.
        /// @param errorCode
        /// @param errorMessage
        /// @return
        static Result<T> Err(int errorCode, const char *errorMessage) { return Result<T>(errorCode, errorMessage); }

        /// @brief Generate a "failure" with an error code and a descriptive message.
        /// @param errorCode
        /// @param errorMessage
        /// @return
        static Result<T> Err(int errorCode, char *errorMessage) { return Result<T>(errorCode, errorMessage); }

        /// @brief "Transpose" a "failure".
        /// @param errorDescription An object which error information should be copied from.
        /// @return
        static Result<T> Err(const ErrorDescription &errorDescription) { return Result<T>(errorDescription.ErrorCode(), errorDescription.ErrorMessage()); }

        /// @brief Returns `true` if the result was "successful".
        /// @return
        bool HasValue() const { return hasValue; }

        /// @brief Returns the success-value or throws an exception if the `Result<T>` was a "failure".
        /// @return
        T Value() const
        {
            if (!HasValue())
            {
                THROW("Unable to get Value. Result<T> was an error and had no value.");
            }

            return value;
        }

        /// @brief Returns the provided error-code or `0` if no error code was provided.
        /// @return
        int ErrorCode() const override { return errorCode; }

        /// @brief Returns an error message if provided.
        /// @return
        const char *ErrorMessage() const override
        {
            if (hasValue)
            {
                return "";
            }
            if (errorMessage == nullptr)
            {
                return "(error)";
            }
            return errorMessage;
        }

        Result(const Result &copy) : value(copy.value), hasValue(copy.hasValue), errorCode(copy.errorCode)
        {
            if (copy.errorMessage != nullptr)
            {
                errorMessage = copy.errorMessage | char_string::retain;
            }
            else
            {
                errorMessage = nullptr;
            }
        }

        ~Result()
        {
            if(errorMessage != nullptr)
            {
                delete []errorMessage;
                errorMessage = nullptr;
            }
        }

        Result<T> *clone() const
        {
            return new Result<T>(*this);
        }

        Result &operator=(const Result &other)
        {
            if (this != &other)
            {
                if (errorMessage != nullptr)
                {
                    delete[] errorMessage;
                }
                errorMessage = other.errorMessage | char_string::retain;
                hasValue = other.hasValue;
                errorCode = other.errorCode;
                value = other.value;
            }
            return *this;
        }

    private:
        Result(int errorCode, bool hasValue) : hasValue(hasValue), errorCode(errorCode), errorMessage("(error)" | char_string::copy) {}
        Result(int errorCode, char *errorMessage) : hasValue(false), errorCode(errorCode), errorMessage(errorMessage | char_string::retain) {}
        Result(int errorCode, const char *errorMessage) : hasValue(false), errorCode(errorCode), errorMessage(errorMessage | char_string::copy) {}

        bool hasValue;
        T value;
        int errorCode;
        char *errorMessage;
    };
}
#endif
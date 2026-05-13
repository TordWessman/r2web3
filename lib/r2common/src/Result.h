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

#ifndef __RESULT_H__
#define __RESULT_H__

#include <cstring>

#ifndef THROW
#ifdef ARDUINO
    void _throwException_arduino(const char *message);
    #define THROW(message) _throwException_arduino(message)
#else
    #include <stdexcept>
    #define THROW(message) throw std::runtime_error(message)
#endif
#endif

namespace r2common
{
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
                errorMessage = _copyString(copy.errorMessage);
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
                errorMessage = _copyString(other.errorMessage);
                hasValue = other.hasValue;
                errorCode = other.errorCode;
                value = other.value;
            }
            return *this;
        }

    private:
        Result(int errorCode, bool hasValue) : hasValue(hasValue), errorCode(errorCode), errorMessage(_copyString("(error)")) {}
        Result(int errorCode, char *errorMessage) : hasValue(false), errorCode(errorCode), errorMessage(_copyString(errorMessage)) {}
        Result(int errorCode, const char *errorMessage) : hasValue(false), errorCode(errorCode), errorMessage(_copyString(errorMessage)) {}

        static char *_copyString(const char *v)
        {
            if (v == nullptr) { return nullptr; }
            char *result = new char[strlen(v) + 1];
            memcpy(result, v, strlen(v));
            result[strlen(v)] = '\0';
            return result;
        }

        bool hasValue;
        T value;
        int errorCode;
        char *errorMessage;
    };
}
#endif

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

#ifndef __TRANSACTION_RESPONSE_H__
#define __TRANSACTION_RESPONSE_H__

#include <cstring>

namespace blockchain
{
    /// @brief Arbitrary response container for requests. Will contain the `"result"` value for an ethereum request.
    class TransactionResponse
    {

    public:
        /// @brief Empty response.
        TransactionResponse() : result(nullptr) {}

        /// @brief Default constructor. `result` must be a null-terminated string.
        TransactionResponse(char *result) : result(result) {}

        TransactionResponse(const TransactionResponse &other)
        {
            if (other.result != nullptr)
            {
                result = new char[strlen(other.result) + 1];
                result[strlen(other.result)] = '\0';
                memcpy(result, other.result, strlen(other.result));
            }
            else
            {
                result = nullptr;
            }
        }

        ~TransactionResponse()
        {
            if (result != nullptr)
            {
                delete[] result;
            }
            result = nullptr;
        }

        TransactionResponse *clone() const { return new TransactionResponse(*this); }

        TransactionResponse &operator=(const TransactionResponse &other)
        {
            if (this != &other)
            {
                if (other.result != nullptr)
                {
                    result = new char[strlen(other.result) + 1];
                    result[strlen(other.result)] = '\0';
                    memcpy(result, other.result, strlen(other.result));
                }
                else
                {
                    result = nullptr;
                }
            }
            return *this;
        }

        /// @brief Returns the (internally managed) `"result"` data.
        /// @return
        char *Result() { return result; }

    private:
        char *result;
    };
}
#endif
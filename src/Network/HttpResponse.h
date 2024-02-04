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

#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include <cstring>
#include "../Shared/Common.h"

namespace blockchain
{
    #define HTTP_OK 200

    /// @brief Response object for HTTP requests.
    class HttpResponse
    {
    public:

        /// @brief Creates a response without a body. `GetBody()` will return `nullptr`.
        HttpResponse(const long status) : status(status), responseLength(0), body(nullptr) { }

        /// @brief Creates a response object. Please note that `responseBody` will be managed by this instance and must therefore not be deallocated separately.
        HttpResponse(const long status, char *responseBody) : 
            status(status),
            responseLength(responseBody != nullptr ? strlen(responseBody) + 1 : 0),
            body(responseBody) { }

        /// @brief Creates a response object by copying the contents of `responseBody`.
        HttpResponse(const long status, const char *responseBody) : 
            status(status),
            responseLength(strlen(responseBody) != 0 ? strlen(responseBody) + 1 : 0)
        {
            body = responseBody | char_string::copy;
        }

        ~HttpResponse()
        {
            if (body != nullptr)
            {
                delete []body;
            }
        }

        HttpResponse &operator=(const HttpResponse &other)
        {
            if (this != &other)
            {
                if (body != nullptr) { delete body; }

                if (other.body != nullptr)
                {
                    body = other.body | char_string::retain;
                }
                else
                {
                    body = nullptr;
                }
            }
            return *this;
        }

        HttpResponse(const HttpResponse &other) :
            status(other.status),
            responseLength((other.body != nullptr && strlen(other.body) != 0) ? strlen(other.body) + 1 : 0)
        {
            if (this != &other)
            {
                if (other.body != nullptr)
                {
                    body = other.body | char_string::retain;
                }
                else
                {
                    body = nullptr;
                }
            }
        }

        /// @brief returns `true` if the response was deemed successfull.
        bool Success() const { return status == HTTP_OK; }
        
        /// @brief Return the content of the response. Please note that the memory will be managed internally.
        char *GetBody() const { return body; }

        /// @brief Return the HTTP status (if available). Otherwise, the value can represent an error code or whatever.
        const long status;

        /// @brief return the length of the response _including_ the null-termination character.
        const size_t responseLength;
        
    private:
        char *body;
    };
}
#endif
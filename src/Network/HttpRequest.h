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

#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "NetworkFacade.h"
#include "HttpResponse.h"

namespace blockchain
{
    /// @brief Represents an object capable of making network requests.
    class HttpRequest
    {

    public:
        /// @brief Construct a request using the provided `NetworkFacade`.
        /// @param network _Will NOT be retained!_
        HttpRequest(const NetworkFacade *network) : network(network){};

        void SetMethod(const char *method) { httpMethod = method; }

        void SetBody(const char *body) { requestBody = body; }

        HttpResponse SendRequest(const char *url) const;

    private:
        const NetworkFacade *network;
        const char *httpMethod;
        const char *requestBody;
    };
}
#endif

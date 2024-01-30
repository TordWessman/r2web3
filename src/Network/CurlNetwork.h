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

#ifndef ARDUINO
#ifndef __CURL_NETWORK_H__
#define __CURL_NETWORK_H__
#include <curl/curl.h>

#include "NetworkFacade.h"
#include "HttpResponse.h"

namespace blockchain
{
    /// @brief Standard, non-micro controller network client.
    class CurlNetwork : public NetworkFacade
    {
    public:
        CurlNetwork();
        ~CurlNetwork();

        HttpResponse MakeRequest(const char *url, const char *method, const char *body) const override;

        CurlNetwork &operator=(const CurlNetwork &) = delete;
        CurlNetwork(const CurlNetwork &other) = delete;

    private:
        CURL *curlHandle;
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output);
    };
}
#endif // __CURL_NETWORK_H__
#endif // ARDUINO

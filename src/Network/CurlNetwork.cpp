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

#include "CurlNetwork.h"
#include "HttpResponse.h"
#include "../Shared/Common.h"

namespace blockchain
{
    CurlNetwork::CurlNetwork()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlHandle = curl_easy_init();
    }

    CurlNetwork::~CurlNetwork()
    {
        if (curlHandle)
        {
            curl_easy_cleanup(curlHandle);
        }
        curl_global_cleanup();
    }

    HttpResponse CurlNetwork::MakeRequest(
        const char *url,
        const char *httpMethod,
        const char *body) const
    {
        if (!curlHandle)
        {
            THROW("Failed to initialize cURL.");
            return HttpResponse(-1);
        }

        curl_easy_setopt(curlHandle, CURLOPT_URL, url);
        curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, httpMethod);
        curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, body);
        struct curl_slist *hs = NULL;
        hs = curl_slist_append(hs, "Content-Type: application/json");
        curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, hs);

        std::string responseBuffer;
        char *response = nullptr;

        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseBuffer);

        CURLcode res = curl_easy_perform(curlHandle);
        if (res != CURLE_OK)
        {
            const char *error = curl_easy_strerror(res);
            response = new char[strlen(error) + 1];
            response[strlen(error)] = '\0';
            return HttpResponse(-1, response);
        }

        long httpCode = 0;
        curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &httpCode);
        response = (char *)malloc(responseBuffer.length() + 1);
        memcpy(response, responseBuffer.c_str(), responseBuffer.length());
        response[responseBuffer.length()] = '\0';
        
        std::cout << "----- RAW RESPONSE: " << std::endl << response << std::endl;
        
        return HttpResponse(httpCode, response);
    }

    size_t CurlNetwork::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
    {
        size_t totalSize = size * nmemb;
        output->append((char *)contents, totalSize);
        return totalSize;
    }
}
#endif
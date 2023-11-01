#ifndef ARDUINO

#include "CurlNetwork.h"
#include "HttpResponse.h"

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

        char *response = nullptr;
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &response);

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

        return HttpResponse(httpCode, response);
    }

    size_t CurlNetwork::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t totalSize = size * nmemb;
        char **responsePtr = static_cast<char **>(userp);

        // Allocate memory for the response buffer
        *responsePtr = static_cast<char *>(realloc(*responsePtr, totalSize + 1));
        if (*responsePtr)
        {
            std::memcpy(*responsePtr, contents, totalSize);
            (*responsePtr)[totalSize] = '\0'; // Null-terminate the string
        }

        return totalSize;
    }
}
#endif
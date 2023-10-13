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
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    };
}
#endif // __CURL_NETWORK_H__
#endif // ARDUINO

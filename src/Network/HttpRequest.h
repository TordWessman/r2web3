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

#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include <cstring>

namespace blockchain
{
    #define HTTP_OK 200

    /// @brief Response object for HTTP requests.
    struct HttpResponse
    {
        HttpResponse(const long status, const char *body) : 
            status(status), 
            body(body != nullptr ? body : ""), 
            responseLength((body != nullptr && body != "") ? strlen(body) + 1 : 0) {}

        const long status;
        const char *body;
        const size_t responseLength;
        bool Success() const { return status == HTTP_OK; }
            
    };
}
#endif
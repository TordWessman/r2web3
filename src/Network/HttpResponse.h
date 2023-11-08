#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include <cstring>

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
            responseLength(strlen(responseBody) + 1),
            body(responseBody) { }

        /// @brief Creates a response object by copying the contents of `responseBody`.
        HttpResponse(const long status, const char *responseBody) : 
            status(status),
            responseLength(strlen(responseBody) != 0 ? strlen(responseBody) + 1 : 0)
        { 
            body = new char[strlen(responseBody) + 1];
            body[strlen(responseBody)] = '\0';
            memcpy(body, responseBody, strlen(responseBody));
        }

        ~HttpResponse()
        {
            if (body != nullptr)
            {
                delete body;
            }
        }

        HttpResponse &operator=(const HttpResponse &other)
        {
            if (this != &other)
            {
                if (body != nullptr) { delete body; }

                if (other.body != nullptr)
                {
                    body = new char[strlen(other.body) + 1];
                    body[strlen(other.body)] = '\0';
                    memcpy(body, other.body, strlen(other.body));
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
                    body = new char[strlen(other.body) + 1];
                    body[strlen(other.body)] = '\0';
                    memcpy(body, other.body, strlen(other.body));
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
#ifndef __NETWORK_FACADE_H__
#define __NETWORK_FACADE_H__

#include "HttpResponse.h"

namespace blockchain
{
    /// @brief Interface for network request. Must be overloaded by concrete platform-specific implementations.
    class NetworkFacade
    {

    public:
        virtual ~NetworkFacade() {}

        /// @brief Execute a request synchronously using the provided parameters.
        /// @param url
        /// @param method
        /// @param body
        /// @return
        virtual HttpResponse MakeRequest(const char *url, const char *method, const char *body) const
    #ifndef ARDUINO
        {
            throw std::runtime_error("NetworkFacade::MakeRequest() must be overloaded");
        }
    #else
            = 0;
    #endif
    };
}
#endif

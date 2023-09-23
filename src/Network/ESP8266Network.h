#ifndef __ESP8266_NETWORK_H__
#define __ESP8266_NETWORK_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "NetworkFacade.h"
#include "HttpResponse.h"

#define ARGH 42

namespace blockchain
{
    class DinMamma {
        public:
            DinMamma() {}
    };
    
    /// @brief Default network client for ESP8266 devices
    class ESP8266Network : public NetworkFacade
    {

    public:
        ESP8266Network();
        HttpResponse MakeRequest(const char *url, const char *method, const char *body) const override;

    private:
        mutable HTTPClient http;
        mutable WiFiClient client;
    };
    
}
#endif
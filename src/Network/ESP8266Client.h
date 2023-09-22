#ifdef ARDUINO

#ifndef __ARDUINO_CLIENT_H__
#define __ARDUINO_CLIENT_H__

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "NetworkFacade.h"
#include "HttpResponse.h"

/// @brief Default network client for ESP8266 devices
class ESP8266Client : public NetworkFacade {

public:
    ESP8266Client();
    HttpResponse MakeRequest(const char *url, const char *method, const char *body) const override;

private:
    mutable HTTPClient http;
    mutable WiFiClient client;
};
#endif
#endif
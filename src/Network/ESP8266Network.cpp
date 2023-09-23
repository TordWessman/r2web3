#include "ESP8266Network.h"
#include <Arduino.h>

namespace blockchain
{
    ESP8266Network::ESP8266Network()
    {
        http.addHeader("Content-Type", "application/json");
    }

    HttpResponse ESP8266Network::MakeRequest(const char *url, const char *method, const char *body) const
    {
        String urlString = url;
        http.begin(client, urlString);
        int httpResponseCode = http.sendRequest(method, (const uint8_t *)body, strlen(body));
        Serial.print("Response code: ");
        Serial.println(httpResponseCode);

        String responseBody = http.getString();
        Serial.println("Response body:");
        Serial.println(responseBody);

        char *responseData = new char[responseBody.length() + 2];
        responseData[responseBody.length() + 1] = '\0';
        responseBody.toCharArray(responseData, responseBody.length() + 1);

        http.end();
        return HttpResponse(httpResponseCode, responseData);
    }
}
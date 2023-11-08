#include "ESPNetwork.h"
#include <Arduino.h>
#include <string.h>

#include "../Shared/Common.h"

namespace blockchain
{

    ESPNetwork::ESPNetwork(const char *certificate)
    {
        ntpServers = {ESPNetwork_NTP_SERVER1, ESPNetwork_NTP_SERVER2, ESPNetwork_NTP_SERVER3};
#ifdef ESP32
        http.setConnectTimeout(ESPNetwork_CONNECTION_TIMEOUT);
#endif
        http.setTimeout(ESPNetwork_CONNECTION_TIMEOUT);
        if (certificate != nullptr) 
        {
#ifdef ESP8266
            BearSSL::WiFiClientSecure *bearSSLClient = new BearSSL::WiFiClientSecure();
            trustedCAs.append(certificate);
            bearSSLClient->setTrustAnchors(&trustedCAs);
            client = bearSSLClient;
#endif
#ifdef ESP32
            client = new WiFiClientSecure();
            ((WiFiClientSecure *)client)->setCACert(certificate);
#endif
            timeSyncRequired = true;
            caCertAdded = true;
        }
        else
        {
            client = new WiFiClient();
            timeSyncRequired = false;
            caCertAdded = false;
        }
    }

    void ESPNetwork::SetNtpServers(const char *ntpServer1, const char *ntpServer2, const char *ntpServer3)
    {
        if (ntpServer1 == nullptr && ntpServer2 == nullptr && ntpServer3 == nullptr)
        {
            ntpServers = {ESPNetwork_NTP_SERVER1, ESPNetwork_NTP_SERVER2, ESPNetwork_NTP_SERVER3};
        }
        else
        {
            ntpServers[0] = ntpServer1;
            ntpServers[1] = ntpServer2;
            ntpServers[2] = ntpServer3;
        }
    }

    bool ESPNetwork::Restart() const
    {
        if (ESPNetwork::SetClock(ntpServers[0], ntpServers[1], ntpServers[2]))
        {
            timeSyncRequired = false;
            return true;
        }
        return false;
    }

    HttpResponse ESPNetwork::MakeRequest(const char *url, const char *method, const char *body) const
    {
        if (timeSyncRequired)
        { 
            if (!Restart()) 
            {
                Log::m("NTP time synchronization failed. Call `Restart` once network is connected.");
                return HttpResponse(-2);
            }
        }

        if (strncmp(url, "https", 5) == 0 && !caCertAdded)
        {
            Log::m("Trying to initiate a https connection without a valid certificate. Instantiate ESPNetwork with a valid root certificate.");
            return HttpResponse(-3);
        }

        http.begin(*client, url);
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.sendRequest(method, (uint8_t *)body, strlen(body));

        if (httpResponseCode < 100)
        {
            Log::m("Connection error with code: ", httpResponseCode);
            return HttpResponse(httpResponseCode);
        }
        else if (httpResponseCode != HTTP_CODE_OK)
        {
            Log::m("Invalid response code: ", httpResponseCode);
            Log::m(http.getString().c_str());
        }

        String responseBody = http.getString();
        // Serial.println("Response body:");
        // Serial.println(responseBody);

        char *responseData = new char[responseBody.length() + 2];
        responseData[responseBody.length() + 1] = '\0';
        responseBody.toCharArray(responseData, responseBody.length() + 1);

        http.end();
        return HttpResponse(httpResponseCode, responseData);
    }

    bool ESPNetwork::SetClock(const char *ntpServer1, const char *ntpServer2, const char *ntpServer3)
    {
        configTime(0, 0, ntpServer1, ntpServer2, ntpServer3);

        Log::m("Waiting for NTP time sync: ");
        time_t now = time(nullptr);
        uint16_t retryCount = 0;

        while (now < 8 * 3600 * 2 && retryCount++ < ESPNetwork_NTP_LOOP_RETRIES)
        {
            delay(ESPNetwork_NTP_LOOP_TIME_MS);
            now = time(nullptr);
        }

        if (retryCount >= ESPNetwork_NTP_LOOP_RETRIES) 
        {
            Log::m("Unable to fetch NPT time");
            return false;
        }

        struct tm timeinfo;
        gmtime_r(&now, &timeinfo);
        Log::m("GMT time: ");
        Log::m(asctime(&timeinfo));
        return true;
    }
}
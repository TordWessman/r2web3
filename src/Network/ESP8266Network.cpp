#include "ESP8266Network.h"
#include <Arduino.h>
#include <string.h>

#include "../Shared/Common.h"

namespace blockchain
{

    ESP8266Network::ESP8266Network(const char *certificate)
    {
        ntpServers = {ESP8266Network_NTP_SERVER1, ESP8266Network_NTP_SERVER2, ESP8266Network_NTP_SERVER3};

        if (certificate != nullptr) 
        {
            BearSSL::WiFiClientSecure *bearSSLClient = new BearSSL::WiFiClientSecure();
            trustedCAs.append(certificate);
            bearSSLClient->setTrustAnchors(&trustedCAs);
            client = bearSSLClient;
            timeSyncRequired = true;
        }
        else
        {
            client = new WiFiClient();
            timeSyncRequired = false;
        }
    }

    ESP8266Network::ESP8266Network(std::vector<const char *> certificates)
    {
        ntpServers = {ESP8266Network_NTP_SERVER1, ESP8266Network_NTP_SERVER2, ESP8266Network_NTP_SERVER3};

        if (certificates.size() > 0)
        {
            BearSSL::WiFiClientSecure *bearSSLClient = new BearSSL::WiFiClientSecure();
            for (const char *certificate : certificates) { trustedCAs.append(certificate); }
            bearSSLClient->setTrustAnchors(&trustedCAs);
            client = bearSSLClient;
            timeSyncRequired = true;
        }
        else
        {
            client = new WiFiClient();
            timeSyncRequired = false;
        }
    }

    void ESP8266Network::SetNtpServers(const char *ntpServer1, const char *ntpServer2, const char *ntpServer3)
    {
        if (ntpServer1 == nullptr && ntpServer2 == nullptr && ntpServer3 == nullptr)
        {
            ntpServers = {ESP8266Network_NTP_SERVER1, ESP8266Network_NTP_SERVER2, ESP8266Network_NTP_SERVER3};
        }
        else
        {
            ntpServers[0] = ntpServer1;
            ntpServers[1] = ntpServer2;
            ntpServers[2] = ntpServer3;
        }
    }

    bool ESP8266Network::Restart() const
    {
        if (ESP8266Network::SetClock(ntpServers[0], ntpServers[1], ntpServers[2]))
        {
            timeSyncRequired = false;
            return true;
        }
        return false;
    }

    HttpResponse ESP8266Network::MakeRequest(const char *url, const char *method, const char *body) const
    {
        if (timeSyncRequired)
        { 
            if (!Restart()) 
            {
                Log::m("NTP time synchronization failed. Call `Restart` once network is connected.");
                return HttpResponse(-2);
            }
        }

        if (strncmp(url, "https", 5) == 0 && trustedCAs.getCount() == 0) 
        {
            Log::m("Trying to initiate a https connection without a valid certificate. Instantiate ESP8266Network with valid root certificate(s).");
            return HttpResponse(-3);
        }
        

        http.begin(*client, url);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.sendRequest(method, (const uint8_t *)body, strlen(body));
        if (httpResponseCode != HTTP_CODE_OK)
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

    bool ESP8266Network::SetClock(const char *ntpServer1, const char *ntpServer2, const char *ntpServer3)
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
#ifndef __ESP8266_NETWORK_H__
#define __ESP8266_NETWORK_H__

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <BearSSLHelpers.h>
#endif
#ifdef ESP32
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#endif

#include <vector>

#include "NetworkFacade.h"
#include "HttpResponse.h"

#define ESPNetwork_NTP_SERVER1 "pool.ntp.org"
#define ESPNetwork_NTP_SERVER2 "time.nist.gov"
#define ESPNetwork_NTP_SERVER3 nullptr
#define ESPNetwork_NTP_LOOP_TIME_MS 500
#define ESPNetwork_NTP_LOOP_RETRIES 50
#define ESPNetwork_CONNECTION_TIMEOUT 1000 * 10

namespace blockchain
{
    /// @brief Default network client for ESP8266 devices
    class ESPNetwork : public NetworkFacade
    {

    public:

        /// @brief Instantiate network client. Please note that `Restart` needs to be called if time sync is lost due to hibernation.
        /// @param certificate Optional root certificate.
        ESPNetwork(const char *certificate = nullptr);

        ~ESPNetwork()
        {
            delete client;
        }

        /// Allows for up to 3 optional NTP servers (ignore if default, hardcoded NTP server defined by `ESPNetwork_NTP_SERVERS` should be used)
        void SetNtpServers(const char *ntpServer1 = nullptr, const char *ntpServer2 = nullptr, const char *ntpServer3 = nullptr);

        /// @brief Invoke once WiFi connection has been re-established (e.g. after hibernation and only required if `ESP6NetworkClient` has been instantiated with certificates).
        /// @return true if NTP time was successfully fetched.
        bool Restart() const;

        HttpResponse MakeRequest(const char *url, const char *method, const char *body) const override;

        // Only allow usage by reference.
        ESPNetwork &operator=(const ESPNetwork &) = delete;
        ESPNetwork(const ESPNetwork &other) = delete;

    private:
        mutable bool timeSyncRequired;
        mutable HTTPClient http;
        mutable WiFiClient *client;
#ifdef ESP8266
        mutable BearSSL::X509List trustedCAs;
#endif
        bool caCertAdded;
        std::vector<const char*> ntpServers;
        static bool SetClock(const char *ntpServer1 = nullptr, const char *ntpServer2 = nullptr, const char *ntpServer3 = nullptr);
    };
    
}
#endif
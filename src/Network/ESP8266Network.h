#ifndef __ESP8266_NETWORK_H__
#define __ESP8266_NETWORK_H__

#include <Arduino.h>
//#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <BearSSLHelpers.h>
#include <vector>

#include "NetworkFacade.h"
#include "HttpResponse.h"

#define ESP8266Network_NTP_SERVER1 "pool.ntp.org"
#define ESP8266Network_NTP_SERVER2 "time.nist.gov"
#define ESP8266Network_NTP_SERVER3 nullptr
#define ESPNetwork_NTP_LOOP_TIME_MS 500
#define ESPNetwork_NTP_LOOP_RETRIES 20

namespace blockchain
{
    /// @brief Default network client for ESP8266 devices
    class ESP8266Network : public NetworkFacade
    {

    public:

        /// @brief Instantiate network client. Please note that `Start` needs to be invoked once WiFi connection has been established.
        /// @param certificate Optional root certificate. 
        ESP8266Network(const char *certificate = nullptr);

        /// @brief Instantiate using multiple certificates.
        /// @param certificates 
        ESP8266Network(std::vector<const char *> certificates);

        ~ESP8266Network() 
        {
            delete client;
        }

        /// @brief Invoke once WiFi connection has been established (only required if `ESP8266Network` has been instantiated with certificates).
        /// Allows for up to 3 optional NTP servers (ignore if default, hardcoded NTP server defined by `ESP8266Network_NTP_SERVERS` should be used)
        /// @return true if NTP time was successfully fetched.
        bool Restart(const char *ntpServer1 = nullptr, const char *ntpServer2 = nullptr, const char *ntpServer3 = nullptr);

        HttpResponse MakeRequest(const char *url, const char *method, const char *body) const override;

        ESP8266Network &operator=(const ESP8266Network &) = delete;
        ESP8266Network(const ESP8266Network &other) = delete;

    private:
        bool started;
        mutable HTTPClient http;
        mutable WiFiClient *client;
        mutable BearSSL::X509List trustedCAs;
        bool SetClock(const char *ntpServer1 = nullptr, const char *ntpServer2 = nullptr, const char *ntpServer3 = nullptr);
    };
    
}
#endif
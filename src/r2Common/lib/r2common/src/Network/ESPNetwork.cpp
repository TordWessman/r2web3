/**
 * MIT License
 *
 * Copyright (c) 2023 Tord Wessman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef ARDUINO
#include "ESPNetwork.h"
#include <Arduino.h>
#include <string.h>

#include "../R2Logger.h"

namespace r2common
{

    uint32_t ESPNetwork::GetUTCTimestamp(const char *ntpServer)
    {
        configTime(0, 0, ntpServer);
        time_t now = time(nullptr);
        uint16_t retryCount = 0;
        while (now < 8 * 3600 * 2 && retryCount++ < ESPNetwork_NTP_LOOP_RETRIES)
        {
            delay(ESPNetwork_NTP_LOOP_TIME_MS);
            now = time(nullptr);
        }
        if (retryCount >= ESPNetwork_NTP_LOOP_RETRIES)
        {
            return 0;
        }
        return (uint32_t)now;
    }

    ESPNetwork::ESPNetwork(const char *certificate, const bool printDebug) : printDebug(printDebug)
    {
        ntpServers = {ESPNetwork_NTP_SERVER1, ESPNetwork_NTP_SERVER2, ESPNetwork_NTP_SERVER3};
#ifdef ESP32
        http.setConnectTimeout(ESPNetwork_CONNECTION_TIMEOUT);
#endif
        http.setTimeout(ESPNetwork_CONNECTION_TIMEOUT);
        if (certificate != nullptr)
        {
            const char *certs[] = {certificate, nullptr};
            initCertificates(certs);
        }
        else
        {
            initCertificates(nullptr);
        }
    }

    ESPNetwork::ESPNetwork(const char **certificates, const bool printDebug) : printDebug(printDebug)
    {
        ntpServers = {ESPNetwork_NTP_SERVER1, ESPNetwork_NTP_SERVER2, ESPNetwork_NTP_SERVER3};
#ifdef ESP32
        http.setConnectTimeout(ESPNetwork_CONNECTION_TIMEOUT);
#endif
        http.setTimeout(ESPNetwork_CONNECTION_TIMEOUT);
        initCertificates(certificates);
    }

    void ESPNetwork::initCertificates(const char **certificates)
    {
        bool hasCerts = certificates != nullptr && certificates[0] != nullptr;

        if (hasCerts)
        {
#ifdef ESP8266
            BearSSL::WiFiClientSecure *bearSSLClient = new BearSSL::WiFiClientSecure();
            for (size_t i = 0; certificates[i] != nullptr; i++)
            {
                trustedCAs.append(certificates[i]);
            }
            bearSSLClient->setTrustAnchors(&trustedCAs);
            client = bearSSLClient;
#endif
#ifdef ESP32
            size_t totalLen = 0;
            for (size_t i = 0; certificates[i] != nullptr; i++)
            {
                totalLen += strlen(certificates[i]);
            }
            concatenatedCerts = new char[totalLen + 1];
            concatenatedCerts[0] = '\0';
            for (size_t i = 0; certificates[i] != nullptr; i++)
            {
                strcat(concatenatedCerts, certificates[i]);
            }
            client = new WiFiClientSecure();
            ((WiFiClientSecure *)client)->setCACert(concatenatedCerts);
#endif
            timeSyncRequired = true;
            caCertAdded = true;
        }
        else
        {
#if defined(ESP32) && R2WEB3_HAS_CERT_BUNDLE
            client = new WiFiClientSecure();
            ((WiFiClientSecure *)client)->setCACertBundle(NULL);
            timeSyncRequired = true;
            caCertAdded = true;
            R2Logger::m("Using Mozilla CA certificate bundle");
#else
            client = new WiFiClient();
            timeSyncRequired = false;
            caCertAdded = false;
#endif
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
                R2Logger::e("NTP time synchronization failed. Call `Restart` once network is connected.");
                return HttpResponse(-2);
            }
        }

        if (strncmp(url, "https", 5) == 0 && !caCertAdded)
        {
            R2Logger::e("Trying to initiate a https connection without a valid certificate. Instantiate ESPNetwork with a root certificate or use Arduino-ESP32 v3.0+ for automatic CA bundle support.");
            return HttpResponse(-3);
        }

        if(printDebug)
        {
            R2Logger::m("Sending request: ", url);
            R2Logger::m("Request body: ", body);
        }

        http.begin(*client, url);
        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.sendRequest(method, (uint8_t *)body, strlen(body));

        if (httpResponseCode < 100)
        {
            R2Logger::e("Connection error with code: ", httpResponseCode);
            return HttpResponse(httpResponseCode);
        }
        else if (httpResponseCode != HTTP_CODE_OK && printDebug)
        {
            R2Logger::e("Invalid response code: ", httpResponseCode);
            if (printDebug) { R2Logger::m(http.getString().c_str()); }
        }

        String responseBody = http.getString();

        if (printDebug) { R2Logger::m("Response body: ", responseBody.c_str()); }

        char *responseData = new char[responseBody.length() + 2];
        responseData[responseBody.length() + 1] = '\0';
        responseBody.toCharArray(responseData, responseBody.length() + 1);

        http.end();
        return HttpResponse(httpResponseCode, responseData);
    }

    bool ESPNetwork::SetClock(const char *ntpServer1, const char *ntpServer2, const char *ntpServer3)
    {
        configTime(0, 0, ntpServer1, ntpServer2, ntpServer3);

        R2Logger::m("Waiting for NTP time sync...");
        time_t now = time(nullptr);
        uint16_t retryCount = 0;

        while (now < 8 * 3600 * 2 && retryCount++ < ESPNetwork_NTP_LOOP_RETRIES)
        {
            delay(ESPNetwork_NTP_LOOP_TIME_MS);
            now = time(nullptr);
        }

        if (retryCount >= ESPNetwork_NTP_LOOP_RETRIES) 
        {
            R2Logger::e("Unable to fetch NPT time");
            return false;
        }

        struct tm timeinfo;
        gmtime_r(&now, &timeinfo);
        R2Logger::m("GMT time: ", asctime(&timeinfo));
        return true;
    }
}
#endif
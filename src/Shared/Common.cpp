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

#include "Common.h"

#include <cctype>
#include <cassert>
#include "../configuration.h"
#include "BigNumber.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifndef ARDUINO
#include <iostream>

unsigned long millis() { return micros() / 1000; }

unsigned long micros()
{
    struct timespec currentTime;
    assert(clock_gettime(CLOCK_REALTIME, &currentTime) == 0);
    return currentTime.tv_sec * 1000000LL + currentTime.tv_nsec / 1000;
}
#endif

namespace blockchain
{

    uint8_t hexCharToByte(const char c)
    {
        if (c >= '0' && c <= '9') { return c - '0'; }
        else if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
        else if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
        THROW("Invalid hex character!");
        return 0;
    }

    namespace byte_array
    {
        std::vector<uint8_t> truncate_t::operator()(const std::vector<uint8_t> &v) const
        {
            size_t zeroCount = 0;
            for (size_t i = 0; i < v.size(); i++)
            {
                if (v[i] == 0)
                {
                    zeroCount++;
                }
                else
                {
                    return std::vector<uint8_t>(v.begin() + zeroCount, v.end());
                }
            }
            
            if (zeroCount == v.size()) 
            {
                return std::vector<uint8_t>();
            }
            
            return v;
        }

        char* hex_string_t::operator()(const std::vector<uint8_t> &v) const
        {
            if (v.size() == 0)
            {
                char *hexString = new char[3];
                memset(hexString, 0, 3);
                return hexString;
            }

            size_t stringLength = v.size() * 2 + 1;
            char *hexString = new char[stringLength];
            hexString[stringLength - 1] = '\0';

            char hexValue[3];
            for (size_t i = 0; i < v.size(); i++)
            {
                snprintf(hexValue, sizeof(hexValue), "%02X", v[i]);
                memcpy(hexString + i * 2, &hexValue, 2);
            }
            return hexString;
        }

        char *to_string_t::operator()(const std::vector<uint8_t> &v) const
        {
            char *str = new char[v.size() + 1];
            str[v.size()] = '\0';

            for (size_t i = 0; i < v.size(); i++)
            {
                str[i] = (char)v[i];
            }
            return str;
        }

        std::vector<uint8_t> hex_string_to_bytes_t::operator()(const char *v) const
        {
            string_info str(v);
            string_info info( str | char_string::remove_hex_prefix);
            std::vector<uint8_t> bytes(info.length / 2 + info.length % 2);
            uint8_t value;
            for(int i = info.begin; i < info.begin + info.length; i += 2)
            {
                if (info.value[i + 1] != '\0')
                {
                    value = (hexCharToByte(info.value[i]) << 4) + hexCharToByte(info.value[i + 1]);
                } 
                else
                {
                    value = hexCharToByte(info.value[i + 1]);
                }
                bytes[(i - info.begin) / 2] = value;
            }
            return bytes;
        }

        std::vector<uint8_t> string_to_bytes_t::operator()(const char *v) const
        {
            std::vector<uint8_t> result(strlen(v));
            for (int i = 0; i < strlen(v); i++)
            {
                result[i] = (uint8_t)v[i];
            }
            return result;
        }

        std::vector<uint8_t> uint_to_bytes_t::operator()(const uint32_t v) const
        {
            const size_t byteCount = sizeof(v);
            std::vector<uint8_t> bytes(byteCount);
            for (size_t i = 0; i < byteCount; ++i)
            {
                bytes[byteCount - i - 1] = static_cast<uint8_t>(v >> (i * 8));
            }
            return bytes | byte_array::truncate;
        }

        std::vector<uint8_t> size_to_bytes_t::operator()(const size_t v) const
        {
            const size_t byteCount = sizeof(v);
            std::vector<uint8_t> bytes(byteCount);
            for (size_t i = 0; i < byteCount; ++i)
            {
                bytes[byteCount - i - 1] = static_cast<uint8_t>(v >> (i * 8));
            }
            return bytes | byte_array::truncate;
        }

        std::vector<uint8_t> operator|(const std::vector<uint8_t> &v, truncate_t f) { return f(v); }
        char* operator|(const std::vector<uint8_t> &v, hex_string_t f) { return f(v); }
        char *operator|(const std::vector<uint8_t> &v, to_string_t f) { return f(v); }
        std::vector<uint8_t> operator|(const char *v, hex_string_to_bytes_t f) { return f(v); }
        std::vector<uint8_t> operator|(const char *v, string_to_bytes_t f) { return f(v); }
        std::vector<uint8_t> operator|(const uint32_t v, uint_to_bytes_t f) { return f(v); }
        std::vector<uint8_t> operator|(const size_t v, size_to_bytes_t f) { return f(v); }
    }

    namespace char_string {

        char* add_hex_prefix_t::operator()(char *v)
        {
            char *result = new char[strlen(v) + 3];
            result[0] = '0'; result[1] = 'x'; result[strlen(v) + 2] = '\0';
            memcpy(result + 2, v, strlen(v));
            delete[] v;
            return result;
        }

        string_info ltrim_t::operator()(const string_info &v)
        {
            size_t begin = 0;
            for (size_t i = 0; i < v.length; i++)
            {
                if (v.value[i] != ' ' && v.value[i] != '\t')
                {
                    return string_info(v.value, i, v.length - i);
                }
            }
            return string_info(v);
        }

        string_info remove_hex_prefix_t::operator()(const string_info &v)
        {
            string_info trimmed = v | ltrim;
            if (trimmed.length < 2)
            {
                return v;
            }
            if (trimmed.value[trimmed.begin] == '0' && (trimmed.value[trimmed.begin + 1] == 'x' || trimmed.value[trimmed.begin + 1] == 'X'))
            {
                return string_info(v.value, v.begin + 2, v.length - 2);
            }
            return string_info(trimmed);
        }

        bool is_null_t::operator()(char *v)
        {
            if (v == NULL) { return true; }
            if (strcmp(v, "null") == 0) { return true; }
            return false;
        }

        char* retain_t::operator()(char *v)
        {
            char *result = new char[strlen(v) + 1];
            memccpy(result, v, 0, strlen(v));
            result[strlen(v)] = '\0';
            return result;
        }

        char *copy_t::operator()(const char *v)
        {
            char *result = new char[strlen(v) + 1];
            memccpy(result, v, 0, strlen(v));
            result[strlen(v)] = '\0';
            return result;
        }

        char* operator|(char *v, add_hex_prefix_t f) { return f(v); }
        string_info operator|(const string_info &v, ltrim_t f) { return f(v); }
        string_info operator|(const string_info &v, remove_hex_prefix_t f) { return f(v); }
        bool operator|(char *v, is_null_t f) { return f(v); }
        char* operator|(char *v, retain_t f) { return f(v); }
        char *operator|(const char *v, copy_t f) { return f(v); }
    }

}

#ifdef ARDUINO

void _throwException_arduino(const char *message)
{
#ifndef R2WEB3_LOGGING_DISABLED
    Serial.println(message);
#endif
    assert(false);
}

#endif
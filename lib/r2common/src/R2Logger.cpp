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

#include "R2Logger.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif

namespace r2common
{
#ifdef R2_LOGGING_DISABLED
    void __r2_printLog(const int logType) {}
    void __r2_printLog(const int logType, const char *m1) {}
    void __r2_printLog(const int logType, const char *m1, const char *m2) {}
    void __r2_printLog(const int logType, const char *m1, char *m2) {}
    void __r2_printLog(const int logType, const char *m1, int m2) {}
    void __r2_printLog(const int logType, const char *m1, uint32_t m2) {}
#else
#ifdef ARDUINO
    void __r2_printError(const int logType) { if (logType == R2Logger::LOG_TYPE_ERROR) { Serial.print("ERROR: "); } }
    void __r2_printLog(const int logType) { __r2_printError(logType); Serial.println(); }
    void __r2_printLog(const int logType, const char *m1) { __r2_printError(logType); Serial.println(m1); }
    void __r2_printLog(const int logType, const char *m1, const char *m2) { __r2_printError(logType); Serial.print(m1); Serial.println(m2); }
    void __r2_printLog(const int logType, const char *m1, char *m2) { __r2_printError(logType); Serial.print(m1); Serial.println(m2); }
    void __r2_printLog(const int logType, const char *m1, int m2) { __r2_printError(logType); Serial.print(m1); Serial.println(m2); }
    void __r2_printLog(const int logType, const char *m1, uint32_t m2) { __r2_printError(logType); Serial.print(m1); Serial.println(m2); }
#else
    void __r2_printLog(const int logType) { std::cout << std::endl; }
    void __r2_printLog(const int logType, const char *m1) { std::cout << m1 << std::endl; }
    void __r2_printLog(const int logType, const char *m1, const char *m2) { std::cout << m1 << " " << m2 << std::endl; }
    void __r2_printLog(const int logType, const char *m1, char *m2) { std::cout << m1 << " " << m2 << std::endl; }
    void __r2_printLog(const int logType, const char *m1, int m2) { std::cout << m1 << " " << m2 << std::endl; }
    void __r2_printLog(const int logType, const char *m1, uint32_t m2) { std::cout << m1 << " " << m2 << std::endl; }
#endif // ARDUINO
#endif // R2_LOGGING_DISABLED

    void R2Logger::log(const int logType)
    {
        if (logType < logLevelThreshold) { return; }
        __r2_printLog(logType);
    }

    void R2Logger::log(const int logType, const char *m1)
    {
        if (logType < logLevelThreshold) { return; }
        __r2_printLog(logType, m1);
        if (delegate) { delegate->log(logType, m1); }
    }

    void R2Logger::log(const int logType, const char *m1, const char *m2)
    {
        if (logType < logLevelThreshold) { return; }
        __r2_printLog(logType, m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }

    void R2Logger::log(const int logType, const char *m1, char *m2)
    {
        if (logType < logLevelThreshold) { return; }
        __r2_printLog(logType, m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }

    void R2Logger::log(const int logType, const char *m1, int m2)
    {
        if (logType < logLevelThreshold) { return; }
        __r2_printLog(logType, m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }

    void R2Logger::log(const int logType, const char *m1, uint32_t m2)
    {
        if (logType < logLevelThreshold) { return; }
        __r2_printLog(logType, m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }
}

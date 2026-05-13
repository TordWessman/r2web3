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

#ifndef __R2_LOGGER_H__
#define __R2_LOGGER_H__

#include <stdint.h>

namespace r2common
{

    /// @brief Implementation will receive log events. Set delegate by calling `R2Logger.getInstance().SetDelegate((R2LoggerDelegate *)delegate)`.
    class R2LoggerDelegate {

    public:
        virtual ~R2LoggerDelegate() { }
        virtual void log(const int logType, const char *m1) { }
        virtual void log(const int logType, const char *m1, const char *m2) { }
        virtual void log(const int logType, const char *m1, char *m2) { }
        virtual void log(const int logType, const char *m1, const int m2) { }
        virtual void log(const int logType, const char *m1, const uint32_t m2) { }
    };

    /// @brief Logger. Allows the framework to print out debug information. Disable tracking by defining `R2_LOGGING_DISABLED`.
    class R2Logger
    {

    public:
        static const int LOG_TYPE_DEBUG = 1;
        static const int LOG_TYPE_MESSAGE = 2;
        static const int LOG_TYPE_ERROR = 3;

        static void m() { getInstance().log(R2Logger::LOG_TYPE_MESSAGE); }
        static void m(const char *m1) { getInstance().log(R2Logger::LOG_TYPE_MESSAGE, m1); }
        static void m(const char *m1, const char *m2) { getInstance().log(R2Logger::LOG_TYPE_MESSAGE, m1, m2); }
        static void m(const char *m1, char *m2) { getInstance().log(R2Logger::LOG_TYPE_MESSAGE, m1, m2); }
        static void m(const char *m1, const int m2) { getInstance().log(R2Logger::LOG_TYPE_MESSAGE, m1, m2); }
        static void m(const char *m1, const uint32_t m2) { getInstance().log(R2Logger::LOG_TYPE_MESSAGE, m1, m2); }

        static void e() { getInstance().log(R2Logger::LOG_TYPE_ERROR); }
        static void e(const char *m1) { getInstance().log(R2Logger::LOG_TYPE_ERROR, m1); }
        static void e(const char *m1, const char *m2) { getInstance().log(R2Logger::LOG_TYPE_ERROR, m1, m2); }
        static void e(const char *m1, char *m2) { getInstance().log(R2Logger::LOG_TYPE_ERROR, m1, m2); }
        static void e(const char *m1, const int m2) { getInstance().log(R2Logger::LOG_TYPE_ERROR, m1, m2); }
        static void e(const char *m1, const uint32_t m2) { getInstance().log(R2Logger::LOG_TYPE_ERROR, m1, m2); }

        static void d(const char *m1, char *m2) { getInstance().log(R2Logger::LOG_TYPE_MESSAGE, m1, m2); }

        /// @brief Enable a delegate callback (class overriding `R2LoggerDelegate`) for whenever a log event is emitted.
        /// @param logDelegate set to `nullptr` in order to remove it.
        void SetDelegate(R2LoggerDelegate *logDelegate) { delegate = logDelegate; }

        /// @brief Set the minimum log level threshold. Only logs at or below this level will be output.
        /// @param level Log level threshold (LOG_TYPE_ERROR=1, LOG_TYPE_MESSAGE=2, LOG_TYPE_DEBUG=3)
        void SetLogLevel(int level) { logLevelThreshold = level; }

        /// @brief Get the current log level threshold.
        /// @return Current log level threshold
        int GetLogLevel() const { return logLevelThreshold; }

        void log(const int logType);
        void log(const int logType, const char *m1);
        void log(const int logType, const char *m1, const char *m2);
        void log(const int logType, const char *m1, char *m2);
        void log(const int logType, const char *m1, const int m2);
        void log(const int logType, const char *m1, const uint32_t m2);

        R2Logger(R2Logger const &) = delete;
        void operator=(R2Logger const &) = delete;

        static R2Logger &getInstance()
        {
            static R2Logger instance;
            return instance;
        }

    private:

        R2LoggerDelegate *delegate;
        int logLevelThreshold;
        R2Logger() : delegate(nullptr), logLevelThreshold(LOG_TYPE_DEBUG) { }
    };
}
#endif

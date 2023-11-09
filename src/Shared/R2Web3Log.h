#ifndef __R2Web3Log_H__
#define __R2Web3Log_H__

#include <stdint.h>

namespace blockchain
{

    /// @brief Implementation will receive log events. Set delegate byt calling `Log.getInstance().SetDelegate((LogDelegate *)delegate)`.
    class LogDelegate {

    public:
        virtual ~LogDelegate() { }
        virtual void log(const int logType, const char *m1) { }
        virtual void log(const int logType, const char *m1, const char *m2) { }
        virtual void log(const int logType, const char *m1, char *m2) { }
        virtual void log(const int logType, const char *m1, const int m2) { }
        virtual void log(const int logType, const char *m1, const uint32_t m2) { }
    };

    /// @brief Logger. Allows the framework to print out debug information. Disable tracking by defining `R2WEB3_LOGGING_DISABLED` in `configuration.h`
    class Log
    {

    public:
        static const int LOG_TYPE_ERROR = 1;
        static const int LOG_TYPE_MESSAGE = 2;

        static void m() { getInstance().log(Log::LOG_TYPE_MESSAGE); }
        static void m(const char *m1) { getInstance().log(Log::LOG_TYPE_MESSAGE, m1); }
        static void m(const char *m1, const char *m2) { getInstance().log(Log::LOG_TYPE_MESSAGE, m1, m2); }
        static void m(const char *m1, char *m2) { getInstance().log(Log::LOG_TYPE_MESSAGE, m1, m2); }
        static void m(const char *m1, const int m2) { getInstance().log(Log::LOG_TYPE_MESSAGE, m1, m2); }
        static void m(const char *m1, const uint32_t m2) { getInstance().log(Log::LOG_TYPE_MESSAGE, m1, m2); }

        static void e() { getInstance().log(Log::LOG_TYPE_ERROR); }
        static void e(const char *m1) { getInstance().log(Log::LOG_TYPE_ERROR, m1); }
        static void e(const char *m1, const char *m2) { getInstance().log(Log::LOG_TYPE_ERROR, m1, m2); }
        static void e(const char *m1, char *m2) { getInstance().log(Log::LOG_TYPE_ERROR, m1, m2); }
        static void e(const char *m1, const int m2) { getInstance().log(Log::LOG_TYPE_ERROR, m1, m2); }
        static void e(const char *m1, const uint32_t m2) { getInstance().log(Log::LOG_TYPE_ERROR, m1, m2); }

        /// @brief Enable a delegate callback (class overriding `LogDelegate`) for whenever a log event is emitted.
        /// @param logDelegate set to `nullptr` in order to remove it.
        void SetDelegate(LogDelegate *logDelegate) { delegate = logDelegate; }

        void log(const int logType);
        void log(const int logType, const char *m1);
        void log(const int logType, const char *m1, const char *m2);
        void log(const int logType, const char *m1, char *m2);
        void log(const int logType, const char *m1, const int m2);
        void log(const int logType, const char *m1, const uint32_t m2);

        Log(Log const &) = delete;
        void operator=(Log const &) = delete;

        static Log &getInstance()
        {
            static Log instance;
            return instance;
        }

    private:

        LogDelegate *delegate;
        Log() : delegate(nullptr) { }
    };
}
#endif
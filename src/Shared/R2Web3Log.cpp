#include "R2Web3Log.h"
#include "../configuration.h"

namespace blockchain
{
#ifdef R2WEB3_LOGGING_DISABLED
    void __r2web3_printLog() { }
    void __r2web3_printLog(const char *m1) { }
    void __r2web3_printLog(const char *m1, const char *m2) { }
    void __r2web3_printLog(const char *m1, char *m2) { }
    void __r2web3_printLog(const char *m1, int m2) { }
    void __r2web3_printLog(const char *m1, uint32_t m2) { }
#else
#ifdef ARDUINO
    void __r2web3_printLog() { Serial.println(); }
    void __r2web3_printLog(const char *m1) { Serial.println(m1); }
    void __r2web3_printLog(const char *m1, const char *m2) { Serial.print(m1); Serial.println(m2); }
    void __r2web3_printLog(const char *m1, char *m2) { Serial.print(m1); Serial.println(m2); }
    void __r2web3_printLog(const char *m1, int m2) { Serial.print(m1); Serial.println(m2); }
    void __r2web3_printLog(const char *m1, uint32_t m2) { Serial.print(m1); Serial.println(m2); }
#else
    void __r2web3_printLog() { std::cout << std::endl; }
    void __r2web3_printLog(const char *m1) { std::cout << m1 << std::endl; }
    void __r2web3_printLog(const char *m1, const char *m2) { std::cout << m1 << " " << m2 << std::endl; }
    void __r2web3_printLog(const char *m1, char *m2) { std::cout << m1 << " " << m2 << std::endl; }
    void __r2web3_printLog(const char *m1, int m2) { std::cout << m1 << " " << m2 << std::endl; }
    void __r2web3_printLog(const char *m1, uint32_t m2) { std::cout << m1 << " " << m2 << std::endl; }
#endif // ARDUINO
#endif // R2WEB3_LOGGING_DISABLED

    void Log::log(const int logType)
    {
        __r2web3_printLog();
    }

    void Log::log(const int logType, const char *m1)
    {
        __r2web3_printLog(m1);
        if (delegate) { delegate->log(logType, m1); }
    }

    void Log::log(const int logType, const char *m1, const char *m2)
    {
        __r2web3_printLog(m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }

    void Log::log(const int logType, const char *m1, char *m2)
    {
        __r2web3_printLog(m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }

    void Log::log(const int logType, const char *m1, int m2)
    {
        __r2web3_printLog(m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }

    void Log::log(const int logType, const char *m1, uint32_t m2)
    {
        __r2web3_printLog(m1, m2);
        if (delegate) { delegate->log(logType, m1, m2); }
    }

    // template <typename T, typename... Args>
    // void Log::m(const T &value, Args... args)
    // {
    //     if (LOGGING_DISABLED) { return; }
    // #ifdef ARDUINO
    //     Serial.print(value, " ");
    // #else
    //     std::cout << value << " ";
    // #endif
    //     Log::m(args...);
    // }
}

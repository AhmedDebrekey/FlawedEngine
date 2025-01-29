#pragma once

#include <string>
#include <mutex>
#include <iostream>

namespace FlawedEngine {
    enum class LogLevel {
        Info,
        Warning,
        Error,
        Debug
    };

    class Logger {
    public:
        static Logger& GetInstance();

        void SetLogLevel(LogLevel level);
        static void Log(const std::string& message, LogLevel level);

    private:
        Logger();
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        void PrintToConsole(const std::string& message, LogLevel level);
        void PrintToFile(const std::string& message, LogLevel level);

        LogLevel m_LogLevel;
        std::mutex m_Mutex;
    };

}

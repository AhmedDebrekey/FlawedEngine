#pragma once

#include <string>
#include <mutex>
#include <iostream>
#include <vector>
#include <chrono>

namespace FlawedEngine {
    enum class LogLevel {
        Info,
        Warning,
        Error,
        Debug,
        Script,
        Console
    };

    struct LogEntry
    {
        std::string Message;
        LogLevel Level;
        std::chrono::system_clock::time_point Timestamp;
    };

    class Logger {
    public:
        static Logger& GetInstance();

        void SetLogLevel(LogLevel level);
        static void Log(const std::string& message, LogLevel level);
        static const std::vector<LogEntry>& GetLogs();
        static void Clear();

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

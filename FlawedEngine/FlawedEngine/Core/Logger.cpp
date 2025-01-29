#include "Logger.h"
namespace FlawedEngine {

    Logger& Logger::GetInstance() {
        static Logger instance;
        return instance;
    }

    Logger::Logger() : m_LogLevel(LogLevel::Info) {}

    Logger::~Logger() {

    }

    void Logger::SetLogLevel(LogLevel level) {
        m_LogLevel = level;
    }

    void Logger::Log(const std::string& message, LogLevel level) {
        Logger& instance = GetInstance();
        std::lock_guard<std::mutex> lock(instance.m_Mutex);

        if (level >= instance.m_LogLevel) {
            instance.PrintToConsole(message, level);
            //instance.PrintToFile(message, level);
        }
    }

    void Logger::PrintToConsole(const std::string& message, LogLevel level) {
        switch (level) {
        case LogLevel::Info:
            std::cout << "\033[32m[INFO]\033[0m " << message << std::endl; // Green
            break;
        case LogLevel::Warning:
            std::cout << "\033[33m[WARNING]\033[0m " << message << std::endl; // Yellow
            break;
        case LogLevel::Error:
            std::cout << "\033[31m[ERROR]\033[0m " << message << std::endl; // Red
            break;
        case LogLevel::Debug:
            std::cout << "\033[36m[DEBUG]\033[0m " << message << std::endl; // Cyan
            break;
        }
    }

}
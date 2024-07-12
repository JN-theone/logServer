#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <mutex>

enum class LogLevel { LOGLEVEL_DEBUG, LOGLEVEL_INFO, LOGLEVEL_WARNING, LOGLEVEL_ERROR };
#define LOGE(...) Logger::getInstance().log(LogLevel::LOGLEVEL_ERROR, __VA_ARGS__)
#define LOGW(...) Logger::getInstance().log(LogLevel::LOGLEVEL_WARNING, __VA_ARGS__)
#define LOGI(...) Logger::getInstance().log(LogLevel::LOGLEVEL_INFO, __VA_ARGS__)

class Logger {

public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

private:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    explicit Logger()
        : level_(LogLevel::LOGLEVEL_INFO)
        , m_mutex()
    {
        std::cout << "Logger construct.\n";
    }

    ~Logger() = default;

public:
    void setLevel(LogLevel level) {
        level_ = level;
    }

    template <typename... Args>
    void log(LogLevel msgLevel, const std::string &format, const Args&... args)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (msgLevel >= level_)
        {
            std::stringstream ss;
            ss << getTime() << " [" << levelToStr(msgLevel) << "]: ";
            // formatString(ss, format, args...);
            if (sizeof...(args) == 0) {
                ss << format;
            }
            else {
                formatString(ss, format, args...);
            }
            std::cout << ss.str() << std::endl;
        }
    }

private:
    std::string getTime()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
        return buf;
    }

    std::string levelToStr(LogLevel level)
    {
        switch (level) {
        case LogLevel::LOGLEVEL_DEBUG:
            return "DEBUG";
        case LogLevel::LOGLEVEL_INFO:
            return "INFO";
        case LogLevel::LOGLEVEL_WARNING:
            return "WARNING";
        case LogLevel::LOGLEVEL_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

    template <typename T, typename... Rest>
    void formatString(std::stringstream& ss, const std::string& format, const T& value, const Rest&... rest) {
        std::size_t pos = format.find_first_of("{");
        if (pos == std::string::npos) {
            ss << format;
            return;
        }
        ss << format.substr(0, pos) << value;
        formatString(ss, format.substr(pos + 2), rest...);
    }

    template <typename... Args>
    void formatString(std::stringstream& ss, const std::string& format, const Args&... args) {
        if (sizeof...(args) == 0) {
            ss << format;
        }
        else {
            formatString(ss, format, args...);
        }
    }

private:
    LogLevel level_;
    std::mutex m_mutex;
};


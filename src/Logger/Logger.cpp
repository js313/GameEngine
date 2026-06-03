#include <iostream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "Logger.h"

std::vector<LogEntry> Logger::messages; // The linker needs a single definition to resolve the memory address. Without std::vector<std::string> Logger::messages; in a compiled .cpp file, the linker fails.

std::string getDateTime()
{
    // Get current time point
    auto now = std::chrono::system_clock::now();

    // Convert to time_t
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // Convert to tm struct (local time)
    std::tm *local_time = std::localtime(&now_time_t);

    // Format using stringstream and put_time
    std::stringstream ss;
    ss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

void Logger::Log(const std::string &message)
{
    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.message = "LOG: [ " + getDateTime() + " ]\t-\t" + message;

    std::cout << "\033[32m" << logEntry.message << "\033[0m";

    messages.push_back(logEntry);
}

void Logger::Err(const std::string &message)
{
    LogEntry logEntry;
    logEntry.type = LOG_ERROR;
    logEntry.message = "ERR: [ " + getDateTime() + " ]\t-\t" + message;

    std::cout << "\033[31m" << logEntry.message << "\033[0m";

    messages.push_back(logEntry);
}
#include <iostream>
#include "Logger.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

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
    std::cout << "\033[32m" << "[ " << getDateTime() << " ]\t-\t" << message << "\033[0m";
}

void Logger::Err(const std::string &message)
{
    std::cout << "\033[31m" << "[ " << getDateTime() << " ]\t-\t" << message << "\033[0m";
}
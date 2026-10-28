#include "utils/DateUtils.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <regex>

namespace medifind::utils {

std::string DateUtils::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tmVal{};
#ifdef _WIN32
    localtime_s(&tmVal, &timeT);
#else
    localtime_r(&timeT, &tmVal);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmVal, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string DateUtils::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tmVal{};
#ifdef _WIN32
    localtime_s(&tmVal, &timeT);
#else
    localtime_r(&timeT, &tmVal);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmVal, "%Y-%m-%d");
    return oss.str();
}

bool DateUtils::isValidDate(const std::string& dateStr) {
    if (dateStr.length() != 10) return false;
    if (dateStr[4] != '-' || dateStr[7] != '-') return false;

    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(dateStr[i]))) return false;
    }

    int year = std::stoi(dateStr.substr(0, 4));
    int month = std::stoi(dateStr.substr(5, 2));
    int day = std::stoi(dateStr.substr(8, 2));

    if (year < 1900 || year > 2200) return false;
    if (month < 1 || month > 12) return false;

    static const int daysInMonths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int maxDays = daysInMonths[month - 1];

    // Leap year check for February
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap) maxDays = 29;
    }

    return (day >= 1 && day <= maxDays);
}

int DateUtils::compareDates(const std::string& date1, const std::string& date2) {
    std::string d1 = date1.substr(0, 10);
    std::string d2 = date2.substr(0, 10);
    if (d1 < d2) return -1;
    if (d1 > d2) return 1;
    return 0;
}

bool DateUtils::isExpired(const std::string& targetDate, const std::string& referenceDate) {
    return compareDates(targetDate, referenceDate) < 0;
}

int DateUtils::daysDifference(const std::string& fromDate, const std::string& toDate) {
    if (!isValidDate(fromDate) || !isValidDate(toDate)) return 0;

    std::tm tmFrom{};
    std::tm tmTo{};

    tmFrom.tm_year = std::stoi(fromDate.substr(0, 4)) - 1900;
    tmFrom.tm_mon  = std::stoi(fromDate.substr(5, 2)) - 1;
    tmFrom.tm_mday = std::stoi(fromDate.substr(8, 2));

    tmTo.tm_year   = std::stoi(toDate.substr(0, 4)) - 1900;
    tmTo.tm_mon    = std::stoi(toDate.substr(5, 2)) - 1;
    tmTo.tm_mday   = std::stoi(toDate.substr(8, 2));

    std::time_t tFrom = std::mktime(&tmFrom);
    std::time_t tTo   = std::mktime(&tmTo);

    double seconds = std::difftime(tTo, tFrom);
    return static_cast<int>(seconds / (60 * 60 * 24));
}

bool DateUtils::isNearExpiry(const std::string& targetDate, const std::string& referenceDate, int warningDays) {
    if (isExpired(targetDate, referenceDate)) return false;
    int daysLeft = daysDifference(referenceDate, targetDate);
    return (daysLeft >= 0 && daysLeft <= warningDays);
}

} // namespace medifind::utils

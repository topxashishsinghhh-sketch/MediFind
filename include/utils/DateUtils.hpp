#pragma once

#include <string>

namespace medifind::utils {

class DateUtils {
public:
    /**
     * @brief Returns current date and time as YYYY-MM-DD HH:MM:SS
     */
    static std::string getCurrentDateTime();

    /**
     * @brief Returns current date as YYYY-MM-DD
     */
    static std::string getCurrentDate();

    /**
     * @brief Validates if string matches valid YYYY-MM-DD format with real calendar days.
     */
    static bool isValidDate(const std::string& dateStr);

    /**
     * @brief Compares two YYYY-MM-DD dates.
     * Returns:
     *   < 0 if date1 < date2
     *   0   if date1 == date2
     *   > 0 if date1 > date2
     */
    static int compareDates(const std::string& date1, const std::string& date2);

    /**
     * @brief Returns true if targetDate < referenceDate.
     */
    static bool isExpired(const std::string& targetDate, const std::string& referenceDate);

    /**
     * @brief Calculates approximate days from referenceDate to targetDate.
     * (targetDate - referenceDate) in days.
     */
    static int daysDifference(const std::string& fromDate, const std::string& toDate);

    /**
     * @brief Returns true if targetDate is not yet expired but expires within warningDays.
     */
    static bool isNearExpiry(const std::string& targetDate, const std::string& referenceDate, int warningDays = 90);
};

} // namespace medifind::utils

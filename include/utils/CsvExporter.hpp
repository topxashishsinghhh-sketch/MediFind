#pragma once

#include <string>
#include <vector>

namespace medifind::utils {

class CsvExporter {
public:
    /**
     * @brief Escapes a field according to RFC 4180 (wraps in quotes if contains comma, quote, or newline).
     */
    static std::string escapeField(const std::string& field);

    /**
     * @brief Exports a 2D table of strings to a CSV file.
     * @param filepath Destination file path.
     * @param headers List of column header names.
     * @param rows 2D matrix of data rows.
     * @return true on success, false on I/O error.
     */
    static bool exportToCsv(const std::string& filepath,
                            const std::vector<std::string>& headers,
                            const std::vector<std::vector<std::string>>& rows);
};

} // namespace medifind::utils

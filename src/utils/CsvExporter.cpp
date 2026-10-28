#include "utils/CsvExporter.hpp"
#include <fstream>
#include <sstream>

namespace medifind::utils {

std::string CsvExporter::escapeField(const std::string& field) {
    bool needsQuotes = false;
    std::string result;
    for (char c : field) {
        if (c == '"') {
            needsQuotes = true;
            result += "\"\""; // escape quote as double quote
        } else {
            if (c == ',' || c == '\n' || c == '\r') {
                needsQuotes = true;
            }
            result += c;
        }
    }
    if (needsQuotes) {
        return "\"" + result + "\"";
    }
    return field;
}

bool CsvExporter::exportToCsv(const std::string& filepath,
                              const std::vector<std::string>& headers,
                              const std::vector<std::vector<std::string>>& rows) {
    std::ofstream outFile(filepath, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        return false;
    }

    // Write UTF-8 BOM for Microsoft Excel compatibility
    outFile << "\xEF\xBB\xBF";

    // Write headers
    for (size_t i = 0; i < headers.size(); ++i) {
        outFile << escapeField(headers[i]);
        if (i + 1 < headers.size()) {
            outFile << ",";
        }
    }
    outFile << "\n";

    // Write rows
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            outFile << escapeField(row[i]);
            if (i + 1 < row.size()) {
                outFile << ",";
            }
        }
        outFile << "\n";
    }

    outFile.close();
    return true;
}

} // namespace medifind::utils

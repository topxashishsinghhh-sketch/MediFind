#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <fstream>
#include <sstream>
#include <mutex>
#include <filesystem>

// CSV utilities with quoted-field and comma support.
namespace csv {

// Parse a single CSV line into fields, handling quoted fields and embedded commas/quotes.
inline std::vector<std::string> parseLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                field += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                fields.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
    }
    fields.push_back(field);
    return fields;
}

// Escape a field for CSV: wrap in quotes if it contains comma, quote, or newline.
inline std::string escapeField(const std::string& value) {
    bool needsQuotes = value.find(',') != std::string::npos ||
                       value.find('"') != std::string::npos ||
                       value.find('\n') != std::string::npos ||
                       value.find('\r') != std::string::npos;
    if (!needsQuotes) return value;
    std::string out = "\"";
    for (char c : value) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    out += "\"";
    return out;
}

// Read all non-empty lines from a file.
inline std::vector<std::string> readLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream in(path);
    if (!in) return lines;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

// Write all lines to a file atomically via temp file + rename.
inline bool writeLines(const std::string& path, const std::vector<std::string>& lines) {
    namespace fs = std::filesystem;
    std::string tmp = path + ".tmp";
    std::ofstream out(tmp);
    if (!out) return false;
    for (size_t i = 0; i < lines.size(); ++i) {
        out << lines[i];
        if (i + 1 < lines.size()) out << "\n";
    }
    out << "\n";
    out.close();
    if (!out) return false;
    try {
        fs::rename(tmp, path);
    } catch (...) {
        return false;
    }
    return true;
}

} // namespace csv

// Base repository for CSV-backed entities.
// T must provide: static std::string csvHeader(); std::string toCsvRow(); static T fromCsvRow(const std::vector<std::string>&); int id();
template<typename T>
class CsvRepository {
protected:
    std::string path;
    std::map<int, T> cache;
    int nextId;
    mutable std::mutex mutex;

    void reload() {
        cache.clear();
        nextId = 1;
        auto lines = csv::readLines(path);
        if (lines.empty()) return;
        // Skip header
        for (size_t i = 1; i < lines.size(); ++i) {
            auto fields = csv::parseLine(lines[i]);
            if (fields.empty()) continue;
            T item = T::fromCsvRow(fields);
            cache[item.id()] = item;
            if (item.id() >= nextId) nextId = item.id() + 1;
        }
    }

    void persist() {
        std::vector<std::string> lines;
        lines.push_back(T::csvHeader());
        for (auto& [id, item] : cache) {
            lines.push_back(item.toCsvRow());
        }
        csv::writeLines(path, lines);
    }

public:
    CsvRepository(const std::string& filePath) : path(filePath), nextId(1) {
        reload();
    }

    std::optional<T> findById(int id) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache.find(id);
        if (it == cache.end()) return std::nullopt;
        return it->second;
    }

    std::vector<T> findAll() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<T> result;
        for (auto& [id, item] : cache) result.push_back(item);
        return result;
    }

    int create(T item) {
        std::lock_guard<std::mutex> lock(mutex);
        item.setId(nextId++);
        cache[item.id()] = item;
        persist();
        return item.id();
    }

    bool update(int id, const T& item) {
        std::lock_guard<std::mutex> lock(mutex);
        if (cache.find(id) == cache.end()) return false;
        cache[id] = item;
        persist();
        return true;
    }

    bool remove(int id) {
        std::lock_guard<std::mutex> lock(mutex);
        if (cache.erase(id)) {
            persist();
            return true;
        }
        return false;
    }

    bool exists(int id) {
        std::lock_guard<std::mutex> lock(mutex);
        return cache.find(id) != cache.end();
    }

    void setNextId(int id) { nextId = id; }
};

#pragma once
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstdint>

// Salted password hasher (no external crypto library dependency).
// Stores credentials as: salt:iterations:hash  (all hex-encoded)
class PasswordHasher {
private:
    static const int DEFAULT_ITERATIONS = 10000;

    static std::string randomSalt(size_t bytes = 16) {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<> dis(0, 255);
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (size_t i = 0; i < bytes; ++i) {
            oss << std::setw(2) << dis(gen);
        }
        return oss.str();
    }

    static std::string hexEncode(const std::string& input) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (unsigned char c : input) {
            oss << std::setw(2) << static_cast<int>(c);
        }
        return oss.str();
    }

    static std::string hexDecode(const std::string& hex) {
        std::string out;
        for (size_t i = 0; i + 1 < hex.size(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            char byte = static_cast<char>(std::stoul(byteString, nullptr, 16));
            out.push_back(byte);
        }
        return out;
    }

    // Simple mixing hash: FNV-1a style over (salt + password) iterated.
    static std::string hashIterations(const std::string& salt, const std::string& password, int iterations) {
        std::string data = salt + password;
        uint64_t h = 14695981039346656037ULL; // FNV offset basis
        for (int i = 0; i < iterations; ++i) {
            for (unsigned char c : data) {
                h ^= c;
                h *= 1099511628211ULL; // FNV prime
            }
            // Re-feed the hash output back into data for next round
            std::ostringstream oss;
            oss << std::hex << h;
            data = oss.str();
        }
        return hexEncode(std::string(reinterpret_cast<const char*>(&h), sizeof(h)));
    }

public:
    // Hash a plaintext password. Returns "salt:iterations:hash".
    static std::string hash(const std::string& password, int iterations = DEFAULT_ITERATIONS) {
        std::string salt = randomSalt();
        std::string hashHex = hashIterations(salt, password, iterations);
        return salt + ":" + std::to_string(iterations) + ":" + hashHex;
    }

    // Verify a plaintext password against a stored "salt:iterations:hash" string.
    static bool verify(const std::string& password, const std::string& stored) {
        size_t first = stored.find(':');
        size_t second = stored.find(':', first + 1);
        if (first == std::string::npos || second == std::string::npos) {
            return false;
        }
        std::string salt = stored.substr(0, first);
        int iterations = std::stoi(stored.substr(first + 1, second - first - 1));
        std::string storedHash = stored.substr(second + 1);
        std::string computed = hashIterations(salt, password, iterations);
        return computed == storedHash;
    }
};

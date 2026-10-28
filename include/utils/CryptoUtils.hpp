#pragma once

#include <string>

namespace medifind::utils {

class CryptoUtils {
public:
    /**
     * @brief Computes standard SHA-256 hash of a string.
     */
    static std::string sha256(const std::string& input);

    /**
     * @brief Generates a cryptographically random hexadecimal salt of given byte length.
     */
    static std::string generateSalt(size_t numBytes = 16);

    /**
     * @brief Hashes a password with salt using 10,000 iterations of SHA-256 (PBKDF-style).
     */
    static std::string hashPassword(const std::string& password, const std::string& salt);

    /**
     * @brief Verifies whether a candidate password matches the stored salt and hash.
     */
    static bool verifyPassword(const std::string& password, const std::string& salt, const std::string& expectedHash);
};

} // namespace medifind::utils

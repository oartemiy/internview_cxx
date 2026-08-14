#include "password.hpp"

#include <sodium.h>

#include <string>

#include "sodium/crypto_pwhash.h"

namespace internview::utils {

std::string HashPassword(const std::string& password) noexcept {
    char hashed_password[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(hashed_password, password.c_str(), password.length(),
                          crypto_pwhash_OPSLIMIT_MIN, crypto_pwhash_MEMLIMIT_MIN) != 0) {
        // NOTE: Extreamly rear situaltion
        return "";
    }
    return std::string{hashed_password};
}

bool VerifyPassword(const std::string& password, const std::string& password_hash) noexcept {
    return crypto_pwhash_str_verify(password_hash.c_str(), password.c_str(), password.length()) ==
           0;
}

}  // namespace internview::utils

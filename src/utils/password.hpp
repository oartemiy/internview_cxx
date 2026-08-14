#pragma once

#include <string>

namespace internview::utils {

std::string HashPassword(const std::string& password);

bool VerifyPassword(const std::string& password, const std::string& password_hash);

}  // namespace internview::utils

#pragma once

#include <boost/uuid.hpp>
#include <optional>
#include <string>

namespace internview::services {

class JwtService {
public:
    JwtService(const std::string& secret_key);

    std::string GenerateToken(const boost::uuids::uuid& user_id, const std::string& role) const;

    std::optional<boost::uuids::uuid> VerifyToken(const std::string& token) const;

private:
    std::string secret_key_;
    std::chrono::seconds token_expiration_;
};

}  // namespace internview::services

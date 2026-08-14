#pragma once

#include <boost/uuid.hpp>
#include <string>

namespace internview::services {

class JwtService {
public:
    JwtService(const std::string& secret_key);

    /**
     * @brief Generates JWT token
     *
     * @param user_id
     * @param role
     * @return std::string
     */
    std::string GenerateToken(const boost::uuids::uuid& user_id, const std::string& role) const noexcept;

    /**
     * @brief Verifies JWT token
     *
     * @param token
     * @return std::pair<boost::uuids::uuid, std::string>
     * @throw userver::server::handlers::Unauthorized
     */
    std::pair<boost::uuids::uuid, std::string> VerifyToken(const std::string& token) const;

private:
    std::string secret_key_;
    std::chrono::seconds token_expiration_;
};

}  // namespace internview::services

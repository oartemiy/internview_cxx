#pragma once

#include <boost/uuid.hpp>
#include <string>

namespace internview::services {

// Use to generate access tokens
class JwtService {
public:
    struct JWTParse {
        boost::uuids::uuid user_id;
        std::string role;
        int password_version;
    };
    JwtService(const std::string& secret_key);

    /**
     * @brief Generates JWT token
     *
     * @param user_id
     * @param role
     * @return std::string
     */
    std::string GenerateToken(const boost::uuids::uuid& user_id, const std::string& role,
                              int password_version) const noexcept;

    /**
     * @brief Verifies JWT token
     *
     * @param token
     * @return JWTParse
     * @throw userver::server::handlers::Unauthorized
     */
    JWTParse VerifyToken(const std::string& token) const;

private:
    std::string secret_key_;
    std::chrono::seconds token_expiration_;
};

}  // namespace internview::services

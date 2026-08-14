#pragma once

#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>

#include "services/jwt_service.hpp"
#include "userver/components/component_context.hpp"

namespace internview::services {

class AuthService {
public:
    struct AuthResult {
        std::string token;
        boost::uuids::uuid user_id;
        std::string role;
    };

    AuthService(const userver::components::ComponentContext& component_context);

    /**
     * @brief Check authoriation
     *
     * @param http_auth_header
     * @return AuthResult
     * @throw userver::server::handlers::Unauthorized
     */
    AuthResult CheckAuthorization(const std::string& http_auth_header) const;

    /**
     * @brief Generated JWT token
     *
     * @param user_id
     * @param role
     * @return std::string
     */
    std::string GenerateJwtToken(const boost::uuids::uuid& user_id, const std::string& role) const {
        return jwt_service_.GenerateToken(user_id, role);
    }

private:
    internview::services::JwtService jwt_service_;
};

}  // namespace internview::services

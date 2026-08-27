#pragma once

#include <memory>
#include <string>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>

#include "services/jwt_service.hpp"
#include "storages/refresh_token_storage.hpp"
#include "userver/components/component_context.hpp"
#include "userver/crypto/hash.hpp"
#include "userver/storages/postgres/cluster.hpp"

namespace internview::services {

class AuthService {
public:
    struct AuthResult {
        std::string access_token;
        boost::uuids::uuid user_id;
        std::string role;
    };

    struct NewTokens {
        std::string access_token;
        std::string refresh_token;
    };


    AuthService(const userver::components::ComponentContext& component_context,
                std::shared_ptr<storages::RefreshTokenStorage> refresh_token_storage_ptr);

    /**
     * @brief Check authoriation
     *
     * @param http_auth_header
     * @return AuthResult
     * @throw userver::server::handlers::Unauthorized
     */
    // TODO: add temporary cache for deleted user (their access token expires in 24 hours)
    AuthResult CheckAuthorization(const std::string& http_auth_header) const;

    

    /**
     * @brief Generated JWT token
     *
     * @param user_id
     * @param role
     * @return std::string
     */
    std::string GenerateAccessToken(const boost::uuids::uuid& user_id, const std::string& role,
                                    int password_version) const noexcept {
        return jwt_service_.GenerateToken(user_id, role, password_version);
    }

    std::string GenerateRefreshToken(const boost::uuids::uuid& user_id) const;

    void RevokeRefreshTokens(const boost::uuids::uuid& user_id) const;

    NewTokens RefreshTokens(const std::string& refresh_token) const;

private:
    internview::services::JwtService jwt_service_;
    std::shared_ptr<internview::storages::RefreshTokenStorage> refresh_token_storage_;
    // TODO: rework auth_service
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace internview::services

#pragma once

#include <dto/user_dto.hpp>
#include <memory>
#include <string>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>

#include "models/user.hpp"
#include "services/jwt_service.hpp"
#include "storages/refresh_token_storage.hpp"
#include "userver/cache/expirable_lru_cache.hpp"
#include "userver/components/component_context.hpp"

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

    struct AuthInfo {
        int password_version;
        std::string role;
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
    AuthResult CheckAuthorization(const std::string& http_auth_header);

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

    /**
     * @brief Generates single refresh token
     *
     * @param user_id
     * @return std::string
     */
    std::string GenerateRefreshToken(const boost::uuids::uuid& user_id) const;

    /**
     * @brief Revokes all user's refresh tokens
     *
     * @param user_id
     */
    void RevokeRefreshTokens(const boost::uuids::uuid& user_id) const;

    /**
     * @brief Generates new access and refresh tokens
     *
     * @param refresh_token
     * @return NewTokens
     */
    NewTokens RefreshTokens(const std::string& refresh_token) const;

    /**
     * @brief Add user to cache to omit database quiry while last access token alive
     *
     * @param user_id
     * @param password_version
     * @param role
     */
    void MarkUserAsChanged(const boost::uuids::uuid& user_id, int password_version,
                           const std::string& role);

    /**
     * @brief Login User
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throws userver::server::handlers::ClientError
     */
    internview::dto::user::ResponseDTO LoginUser(const internview::dto::user::LoginDTO& dto);

    /**
     * @brief Change user's password
     *
     * @param dto
     * @throws userver::server::handlers::ResourceNotFound
               userver::server::handlers::ClientError
     */
    void ChangeUserPassword(const dto::user::ChangePasswordDTO& dto);

    /**
     * @brief Create a User object
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throws userver::server::handlers::ConflictError
               std::runtime_error
     */
    dto::user::ResponseDTO Register(const internview::dto::user::CreateDTO& dto);

private:
    internview::services::JwtService jwt_service_;
    std::shared_ptr<internview::storages::RefreshTokenStorage> refresh_token_storage_;

    userver::storages::postgres::ClusterPtr pg_cluster_;
    userver::cache::ExpirableLruCache<boost::uuids::uuid, AuthInfo> cache_;
    userver::engine::TaskProcessor& crypto_tp_;

    models::User GetUserById(const boost::uuids::uuid& id);
};

}  // namespace internview::services

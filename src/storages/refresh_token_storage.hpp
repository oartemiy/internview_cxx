#pragma once

#include <chrono>
#include <optional>
#include <userver/storages/postgres/cluster.hpp>

#include "models/refresh_token.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

namespace internview::storages {

class RefreshTokenStorage {
public:
    struct NewRefreshToken {
        std::string token;
        boost::uuids::uuid user_id;
    };

    explicit RefreshTokenStorage(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context,
                                 std::chrono::seconds limit = std::chrono::days(14));

    /**
     * @brief Creates refresh token in database
     *
     * @param user_id
     * @param token_hash
     */
    void Create(const boost::uuids::uuid& user_id, const std::string& token_hash) const;

    /**
     * @brief Get the By Token Hash object
     *
     * @param token_hash
     * @return std::optional<models::RefreshToken>
     */
    std::optional<models::RefreshToken> GetByTokenHash(const std::string& token_hash) const;

    /**
     * @brief Revokes token by id
     *
     * @param id
     */
    void Revoke(const boost::uuids::uuid& id) const;

    /**
     * @brief Revokes all user's tokens
     *
     * @param user_id
     */
    void RevokeAllUserTokens(const boost::uuids::uuid& user_id) const;

    /**
     * @brief Deletes token from database
     *
     * @param id
     */
    void DeleteToken(const boost::uuids::uuid& id) const;

    /**
     * @brief Generates new refresh token by previous token
     *
     * @param refresh_token
     * @return NewRefreshToken
     */
    NewRefreshToken RefreshToken(const std::string& refresh_token) const;

    void ClearExpiredTokens() const;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::chrono::seconds limit_;
};

}  // namespace internview::storages

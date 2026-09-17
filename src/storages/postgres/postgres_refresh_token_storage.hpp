#pragma once

#include "storages/interfaces/refresh_token_storage.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

namespace internview::storages::postgres {

class PostgresRefreshTokenStorage final
    : public internview::storages::interfaces::IRefreshTokenStorage {
public:
    PostgresRefreshTokenStorage(const userver::components::ComponentConfig& config,
                                const userver::components::ComponentContext& component_context);

    ~PostgresRefreshTokenStorage() override = default;
    PostgresRefreshTokenStorage(const PostgresRefreshTokenStorage&) = delete;
    PostgresRefreshTokenStorage& operator=(const PostgresRefreshTokenStorage&) = delete;
    PostgresRefreshTokenStorage(PostgresRefreshTokenStorage&&) = delete;
    PostgresRefreshTokenStorage& operator=(PostgresRefreshTokenStorage&&) = delete;

    /**
     * @brief Creates refresh token in database
     *
     * @param user_id
     * @param token_hash
     */
    void Create(const boost::uuids::uuid& user_id, const std::string& token_hash) override;

    /**
     * @brief Get the By Token Hash object
     *
     * @param token_hash
     * @return std::optional<models::RefreshToken>
     */
    std::optional<models::RefreshToken> GetByTokenHash(const std::string& token_hash) override;

    /**
     * @brief Revokes token by id
     *
     * @param id
     */
    void Revoke(const boost::uuids::uuid& id) override;

    /**
     * @brief Revokes all user's tokens
     *
     * @param user_id
     */
    void RevokeAllUserTokens(const boost::uuids::uuid& user_id) override;

    /**
     * @brief Deletes token from database
     *
     * @param id
     */
    void DeleteToken(const boost::uuids::uuid& id) override;

    /**
     * @brief Generates new refresh token by previous token
     *
     * @param refresh_token
     * @return NewRefreshToken
     */
    NewRefreshToken RefreshToken(const std::string& refresh_token) override;

    /**
     * @brief Clears expired tokens
     *
     */
    void ClearExpiredTokens() override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::chrono::seconds limit_;
};

}  // namespace internview::storages::postgres

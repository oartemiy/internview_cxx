#pragma once

#include <chrono>
#include <optional>
#include <userver/storages/postgres/cluster.hpp>

#include "models/refresh_token.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

namespace internview::storages {

// TODO: add documentation
class RefreshTokenStorage {
public:
    explicit RefreshTokenStorage(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context,
                                 std::chrono::seconds limit = std::chrono::days(14));

    void Create(const boost::uuids::uuid& user_id, const std::string& token_hash) const;

    std::optional<models::RefreshToken> GetByTokenHash(const std::string& token_hash) const;

    void Revoke(const boost::uuids::uuid& id) const;

    void RevokeAllUserTokens(const boost::uuids::uuid& user_id) const;

    // TODO: First thing to do
    models::RefreshToken RefreshRefreshToken(const std::string& );

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::chrono::seconds limit_;
};

}  // namespace internview::storages

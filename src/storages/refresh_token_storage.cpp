#include "refresh_token_storage.hpp"

#include <__chrono/duration.h>

#include <chrono>
#include <userver/storages/postgres/exceptions.hpp>

#include "models/refresh_token.hpp"
#include "refresh_token_storage_queries/sql_queries.hpp"
#include "userver/crypto/base64.hpp"
#include "userver/crypto/hash.hpp"
#include "userver/crypto/random.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/chrono.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/utils/boost_uuid7.hpp"

namespace internview::storages {

RefreshTokenStorage::RefreshTokenStorage(
    [[maybe_unused]] const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context, std::chrono::seconds limit)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      limit_(limit) {
}

void RefreshTokenStorage::Create(const boost::uuids::uuid& user_id,
                                 const std::string& token_hash) const {
    boost::uuids::uuid id = userver::utils::generators::GenerateBoostUuidV7();
    auto expires_at =
        userver::storages::postgres::TimePointTz{std::chrono::system_clock::now() + limit_};
    try {
        auto pg_res = pg_cluster_->Execute(
            userver::v3_1::storages::postgres::ClusterHostType::kMaster,
            refresh_token_storage_queries::sql::kCreate, id, user_id, token_hash, expires_at);
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "User deleted on does not exists"));
    }
}

std::optional<models::RefreshToken> RefreshTokenStorage::GetByTokenHash(
    const std::string& token_hash) const {

    auto pg_res =
        pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kSlave,
                             refresh_token_storage_queries::sql::kGetByTokenHash, token_hash);
    if (pg_res.IsEmpty()) {
        return std::nullopt;
    }
    return pg_res.AsSingleRow<models::RefreshToken>(userver::v3_1::storages::postgres::kRowTag);
}

void RefreshTokenStorage::Revoke(const boost::uuids::uuid& id) const {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                       refresh_token_storage_queries::sql::kRevoke, id);
}

void RefreshTokenStorage::RevokeAllUserTokens(const boost::uuids::uuid& user_id) const {
    auto pg_res =
        pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                             refresh_token_storage_queries::sql::kRevokeAllUserTokens, user_id);
}

void RefreshTokenStorage::DeleteToken(const boost::uuids::uuid& id) const {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                       refresh_token_storage_queries::sql::kDelete, id);
}

RefreshTokenStorage::NewRefreshToken RefreshTokenStorage::RefreshToken(
    const std::string& refresh_token) const {
    auto token_hash = userver::crypto::hash::Sha256(refresh_token);
    auto token_opt = GetByTokenHash(token_hash);
    if (token_opt) {
        auto model = *token_opt;
        if (model.expires_at <= std::chrono::system_clock::now() || model.revoked) {
            throw userver::server::handlers::Unauthorized(userver::formats::json::MakeObject(
                "message",
                "This refresh token has expired or revoked, use login handler to update tokens"));
        }
        DeleteToken(model.id);
        auto new_token =
            userver::crypto::base64::Base64Encode(userver::crypto::GenerateRandomBlock(32));
        auto new_token_hash = userver::crypto::hash::Sha256(new_token);
        Create(model.user_id, new_token_hash);
        return {new_token, model.user_id};
    } else {
        throw userver::server::handlers::Unauthorized(userver::formats::json::MakeObject(
            "message", "This refresh token does not exists, use login handler to update tokens"));
    }
}

}  // namespace internview::storages

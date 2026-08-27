#include "auth_service.hpp"

#include <string_view>
#include <userver/storages/secdist/provider_component.hpp>

#include "components/internview_component.hpp"
#include "userver/crypto/base64.hpp"
#include "userver/crypto/hash.hpp"
#include "userver/crypto/random.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/utils/boost_uuid7.hpp"

namespace internview::services {

AuthService::AuthService(const userver::components::ComponentContext& component_context,
                         std::shared_ptr<storages::RefreshTokenStorage> refresh_token_storage_ptr)
    : jwt_service_(component_context
                       .FindComponent<userver::components::DefaultSecdistProvider>(
                           "default-secdist-provider")
                       .Get()
                       .As<userver::formats::json::Value>()["jwt_secret"]
                       .As<std::string>()),
      refresh_token_storage_(refresh_token_storage_ptr),
      pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()) {
}

AuthService::AuthResult AuthService::CheckAuthorization(const std::string& http_auth_header) const {
    if (http_auth_header.empty()) {
        throw userver::server::handlers::Unauthorized(
            userver::formats::json::MakeObject("message", "No authorization"));
    }
    if (!http_auth_header.starts_with("Bearer ")) {
        throw userver::server::handlers::Unauthorized(
            userver::formats::json::MakeObject("message", "Invalid authorization type"));
    }
    std::string token = http_auth_header.substr(7);
    if (token.empty()) {
        throw userver::server::handlers::Unauthorized(
            userver::formats::json::MakeObject("message", "Empty JWT token"));
    }
    auto [id, role, version] = jwt_service_.VerifyToken(token);
    // TODO: make autorization without queries to db
    auto pg_res = pg_cluster_->Execute(
        userver::v3_1::storages::postgres::ClusterHostType::kSlave,
        "SELECT password_version FROM internview_schema.users WHERE id = $1", id);
    int password_version = pg_res.AsSingleRow<int>();
    if (password_version == version) {
        return {token, id, role};
    } else {
        throw userver::server::handlers::Unauthorized(userver::formats::json::MakeObject(
            "message", "User has changed password, this access token has expired"));
    }
}

std::string AuthService::GenerateRefreshToken(const boost::uuids::uuid& user_id) const {
    auto token = userver::crypto::base64::Base64Encode(userver::crypto::GenerateRandomBlock(32));
    auto token_hash = userver::crypto::hash::Sha256(token);

    refresh_token_storage_->Create(user_id, token_hash);

    return token;
}

void AuthService::RevokeRefreshTokens(const boost::uuids::uuid& user_id) const {
    refresh_token_storage_->RevokeAllUserTokens(user_id);
}

AuthService::NewTokens AuthService::RefreshTokens(const std::string& refresh_token) const {
    auto new_refresh_token = refresh_token_storage_->RefreshToken(refresh_token);
    auto pg_res = pg_cluster_->Execute(
        userver::v3_1::storages::postgres::ClusterHostType::kSlave,
        "SELECT role, password_version FROM internview_schema.users WHERE id = $1",
        new_refresh_token.user_id);
    auto role = pg_res[0][0].As<std::string>();
    auto password_version = pg_res[0][1].As<int>();
    return {new_refresh_token.token,
            GenerateAccessToken(new_refresh_token.user_id, role, password_version)};
}

}  // namespace internview::services

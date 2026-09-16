#include "auth_service.hpp"

#include <chrono>
#include <string_view>
#include <userver/storages/secdist/provider_component.hpp>

#include "models/user.hpp"
#include "storages/sql/user_storage/include/user_storage_queries/sql_queries.hpp"
#include "userver/crypto/base64.hpp"
#include "userver/crypto/hash.hpp"
#include "userver/crypto/random.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "utils/password.hpp"
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
                      .GetCluster()),
      cache_(16, 256),
      crypto_tp_(component_context.GetTaskProcessor("crypt-task-processor")) {
    // !NOTE:
    cache_.SetMaxLifetime(std::chrono::minutes(30));  // = access_token_lifetime
}

// NOTE: copy of user storage GetUserById
models::User AuthService::GetUserById(const boost::uuids::uuid& id) {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                       user_storage_queries::sql::kGetUserById, id);

    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ResourceNotFound(userver::formats::json::MakeObject(
            "message", "User with id: " + boost::uuids::to_string(id) + " not found"));
    }
    auto user = pg_res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);
    return user;
}

AuthService::AuthResult AuthService::CheckAuthorization(const std::string& http_auth_header) {
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
    auto auth_cache_info = cache_.GetOptionalNoUpdate(id);
    if (!auth_cache_info) {
        return {token, id, role};
    } else {
        if (auth_cache_info->password_version == version) {
            return {token, id, role};
        } else {
            throw userver::server::handlers::Unauthorized(userver::formats::json::MakeObject(
                "message", "User has changed password, this access token has expired"));
        }
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

void AuthService::MarkUserAsChanged(const boost::uuids::uuid& user_id, int password_version,
                                    const std::string& role) {
    cache_.Put(user_id, {password_version, role});
}

internview::dto::user::ResponseDTO AuthService::LoginUser(
    const internview::dto::user::LoginDTO& dto) {

    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                       user_storage_queries::sql::kLoginUser, dto.login);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "User with login: " + dto.login + " not found"));
    }
    auto user = pg_res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);

    auto verify_res_fut = userver::engine::AsyncNoTracing(crypto_tp_, [&dto, &user] {
        return internview::utils::VerifyPassword(dto.password, user.password_hash);
    });

    auto verify_res = verify_res_fut.Get();

    if (!verify_res) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Password is incorrect"));
    }
    auto access_token = GenerateAccessToken(user.id, user.role, user.password_version);
    auto refresh_token = GenerateRefreshToken(user.id);
    auto resp_dto = dto::user::ResponseDTO{user.id,         user.login,       user.name,
                                           user.role,       user.description, user.profile_pic,
                                           user.created_at, access_token,     refresh_token};
    return resp_dto;
}

void AuthService::ChangeUserPassword(const dto::user::ChangePasswordDTO& dto) {
    auto user = GetUserById(dto.id);

    auto verify_res_fut = userver::engine::AsyncNoTracing(crypto_tp_, [&dto, &user] {
        return internview::utils::VerifyPassword(dto.old_password, user.password_hash);
    });

    auto verify_res = verify_res_fut.Get();

    if (!verify_res) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Password is incorrect"));
    }

    auto new_password_hash_fut = userver::engine::AsyncNoTracing(
        crypto_tp_, [&dto]() { return internview::utils::HashPassword(dto.new_password); });

    auto new_password_hash = new_password_hash_fut.Get();

    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       user_storage_queries::sql::kChangeUserPassword, dto.id,
                                       new_password_hash);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "User with login: " + user.login + " not found"));
    }
    RevokeRefreshTokens(dto.id);
    // Mark user as changed
    MarkUserAsChanged(dto.id, user.password_version + 1, user.role);
}

dto::user::ResponseDTO AuthService::Register(const internview::dto::user::CreateDTO& dto) {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    auto password_hash_fut = userver::engine::AsyncNoTracing(
        crypto_tp_, [&dto]() { return internview::utils::HashPassword(dto.password); });

    auto password_hash = password_hash_fut.Get();
    if (password_hash.empty()) {
        throw std::runtime_error("Sodium error");
    }
    try {
        auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                           user_storage_queries::sql::kCreateUser, id, dto.login,
                                           password_hash, dto.name, dto.role, dto.description,
                                           dto.profile_pic);

        auto resp_dto =
            dto::user::ResponseDTO{id,
                                   dto.login,
                                   dto.name,
                                   dto.role,
                                   dto.description,
                                   dto.profile_pic,
                                   pg_res[0][0].As<std::chrono::system_clock::time_point>(),
                                   GenerateAccessToken(id, dto.role, 0),
                                   GenerateRefreshToken(id)};
        return resp_dto;
    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Login: " + dto.login + " is taken. Try another one"));
    }
}

}  // namespace internview::services

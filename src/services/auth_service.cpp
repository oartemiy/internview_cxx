#include "auth_service.hpp"

#include <chrono>
#include <string_view>
#include <userver/storages/secdist/provider_component.hpp>

#include "components/storages/refresh_token_storage_component.hpp"
#include "components/storages/user_storage_component.hpp"
#include "models/user.hpp"
#include "userver/crypto/base64.hpp"
#include "userver/crypto/hash.hpp"
#include "userver/crypto/random.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include "utils/password.hpp"

namespace internview::services {

AuthService::AuthService([[maybe_unused]] const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
    : jwt_service_(component_context
                       .FindComponent<userver::components::DefaultSecdistProvider>(
                           "default-secdist-provider")
                       .Get()
                       .As<userver::formats::json::Value>()["jwt_secret"]
                       .As<std::string>()),
      refresh_token_storage_(
          component_context.FindComponent<internview::components::RefreshTokenStorage>()
              .GetStorage()),
      user_storage_(component_context.FindComponent<internview::components::UserStorageComponent>()
                        .GetStorage()),
      cache_(16, 256),
      crypto_tp_(component_context.GetTaskProcessor("crypt-task-processor")) {
    // !NOTE:
    cache_.SetMaxLifetime(std::chrono::minutes(30));  // = access_token_lifetime
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
            return {std::move(token), id, std::move(role)};
        } else {
            throw userver::server::handlers::Unauthorized(userver::formats::json::MakeObject(
                "message", "User has changed password, this access token has expired"));
        }
    }
}

std::string AuthService::GenerateRefreshToken(const boost::uuids::uuid& user_id) const {
    auto token = userver::crypto::base64::Base64Encode(userver::crypto::GenerateRandomBlock(32));
    auto token_hash = userver::crypto::hash::Sha256(token);

    refresh_token_storage_->Create(user_id, std::move(token_hash));

    return token;
}

void AuthService::Revoke(const boost::uuids::uuid& user_id) const {
    refresh_token_storage_->RevokeAllUserTokens(user_id);
}

AuthService::NewTokens AuthService::Refresh(const std::string& refresh_token) const {
    auto new_refresh_token = refresh_token_storage_->RefreshToken(refresh_token);
    auto user = user_storage_->GetUserById(new_refresh_token.user_id);

    return {std::move(new_refresh_token.token),
            GenerateAccessToken(new_refresh_token.user_id, user.role, user.password_version)};
}

void AuthService::MarkAsChanged(const boost::uuids::uuid& user_id, int password_version,
                                const std::string& role) {
    cache_.Put(user_id, {password_version, role});
}

internview::dto::user::ResponseDTO AuthService::Login(const internview::dto::user::LoginDTO& dto) {

    auto user = user_storage_->GetUserByLogin(dto.login);

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
    auto resp_dto = dto::user::ResponseDTO{user.id,         std::move(user.login),       std::move(user.name),
                                           std::move(user.role),       std::move(user.description), std::move(user.profile_pic),
                                           user.created_at, std::move(access_token),     std::move(refresh_token)};
    return resp_dto;
}

void AuthService::ChangePassword(const dto::user::ChangePasswordDTO& dto) {
    const auto user = user_storage_->GetUserById(dto.id);

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

    user_storage_->UpdatePasswordHash(dto.id, new_password_hash);

    Revoke(dto.id);
    // Mark user as changed
    MarkAsChanged(dto.id, user.password_version + 1, user.role);
}

dto::user::ResponseDTO AuthService::Register(const internview::dto::user::RegisterDTO& dto) {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    auto password_hash_fut = userver::engine::AsyncNoTracing(
        crypto_tp_, [&dto]() { return internview::utils::HashPassword(dto.password); });

    auto password_hash = password_hash_fut.Get();
    if (password_hash.empty()) {
        throw std::runtime_error("Sodium error");
    }
    auto resp_dto = user_storage_->CreateUser(
        {id, dto.login, password_hash, dto.name, dto.role, dto.description, dto.profile_pic});
    resp_dto.access_token = GenerateAccessToken(id, dto.role, 0);
    resp_dto.refresh_token = GenerateRefreshToken(id);
    return resp_dto;
}

}  // namespace internview::services

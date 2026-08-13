#include "auth_service.hpp"

#include <expected>
#include <string_view>
#include <userver/storages/secdist/provider_component.hpp>

#include "userver/formats/json/value.hpp"

namespace internview::services {

AuthService::AuthService(const userver::components::ComponentContext& component_context)
    : jwt_service_(component_context
                       .FindComponent<userver::components::DefaultSecdistProvider>(
                           "default-secdist-provider")
                       .Get()
                       .As<userver::formats::json::Value>()["jwt_secret"]
                       .As<std::string>()) {
}

std::string AuthService::GetToken(const std::string& http_auth_header) const {
    return http_auth_header.substr(7);
}

std::expected<boost::uuids::uuid, AuthService::AuthError> AuthService::IsAuthorized(
    const std::string& http_auth_header) const {
    if (http_auth_header.empty()) {
        return std::unexpected{AuthError::kEmptyHeader};
    }
    if (http_auth_header.size() < 7 || !http_auth_header.starts_with("Bearer ")) {
        return std::unexpected{AuthError::kInvalidAuthType};
    }
    std::string token = http_auth_header.substr(7);
    if (token.empty()) {
        return std::unexpected{AuthError::kEmptyToken};
    }
    if (auto id = jwt_service_.VerifyToken(token); id) {
        return *id;
    }
    return std::unexpected{AuthError::kUnauthorized};
}

}  // namespace internview::services

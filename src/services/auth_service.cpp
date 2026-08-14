#include "auth_service.hpp"

#include <string_view>
#include <userver/storages/secdist/provider_component.hpp>

#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/exceptions.hpp"

namespace internview::services {

AuthService::AuthService(const userver::components::ComponentContext& component_context)
    : jwt_service_(component_context
                       .FindComponent<userver::components::DefaultSecdistProvider>(
                           "default-secdist-provider")
                       .Get()
                       .As<userver::formats::json::Value>()["jwt_secret"]
                       .As<std::string>()) {
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
    auto [id, role] = jwt_service_.VerifyToken(token);

    return {token, id, role};
}

}  // namespace internview::services

#pragma once

#include <expected>

#include "services/jwt_service.hpp"
#include "userver/components/component_context.hpp"
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace internview::services {

class AuthService {
public:
    enum class AuthError { kUnauthorized, kEmptyHeader, kEmptyToken, kInvalidAuthType };

    AuthService(const userver::components::ComponentContext& component_context);

    std::expected<boost::uuids::uuid, AuthError> IsAuthorized(
        const std::string& http_auth_header) const;

    std::string GenerateJwtToken(const boost::uuids::uuid& user_id, const std::string& role) const {
        return jwt_service_.GenerateToken(user_id, role);
    }

    std::string GetToken(const std::string& http_auth_header) const;

private:
    internview::services::JwtService jwt_service_;
};

inline auto GetJSON(const std::expected<boost::uuids::uuid, AuthService::AuthError>& expected) {
    userver::formats::json::ValueBuilder builder;
    switch (expected.error()) {
        case AuthService::AuthError::kEmptyHeader:
            builder["error"] = "No authorization";
            return builder.ExtractValue();
        case AuthService::AuthError::kEmptyToken:
            builder["error"] = "Empty token";
            return builder.ExtractValue();
        case AuthService::AuthError::kInvalidAuthType:
            builder["error"] = "Invalid autorization type";
            return builder.ExtractValue();
        case AuthService::AuthError::kUnauthorized:
            builder["error"] = "Unauthorized: Invalid JWT or expired token";
            return builder.ExtractValue();
    }
}

}  // namespace internview::services

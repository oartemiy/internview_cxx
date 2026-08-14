#include "jwt_service.hpp"

#include <jwt-cpp/jwt.h>

#include <chrono>
#include <exception>
#include <string>
#include <userver/server/handlers/exceptions.hpp>

#include "jwt-cpp/traits/kazuho-picojson/defaults.h"

namespace internview::services {

JwtService::JwtService(const std::string& secret_code)
    : secret_key_(secret_code), token_expiration_(std::chrono::hours(24)) {
}

std::string JwtService::GenerateToken(const boost::uuids::uuid& user_id,
                                      const std::string& role) const {
    const std::string user_id_str = boost::uuids::to_string(user_id);

    auto token = jwt::create()
                     .set_type("JWT")
                     .set_issuer("internview")
                     .set_issued_now()
                     .set_expires_in(token_expiration_)
                     .set_payload_claim("user_id", jwt::claim(user_id_str))
                     .set_payload_claim("role", jwt::claim(role))  // Optimize queries to db
                     .sign(jwt::algorithm::hs256{secret_key_});
    return token;
}

std::pair<boost::uuids::uuid, std::string> JwtService::VerifyToken(const std::string& token) const {
    auto decoded = jwt::decode(token);
    auto verifier =
        jwt::verify().allow_algorithm(jwt::algorithm::hs256{secret_key_}).with_issuer("internview");

    try {
        verifier.verify(decoded);
    } catch (std::exception& e) {
        throw userver::server::handlers::Unauthorized(userver::formats::json::MakeObject(
            "message", "Unauthorized: Invalid JWT token or expired token"));
    }

    auto user_id_claim = decoded.get_payload_claim("user_id");
    auto user_role_claim = decoded.get_payload_claim("role");

    std::string user_id_str = user_id_claim.as_string();
    std::string user_role_str = user_role_claim.as_string();
    boost::uuids::string_generator gen;
    auto user_id = gen(user_id_str);

    return {user_id, user_role_str};
}

}  // namespace internview::services

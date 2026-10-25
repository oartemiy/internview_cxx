#include "services/jwt_service.hpp"

#include <jwt-cpp/jwt.h>

#include <chrono>
#include <string>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/utest/utest.hpp>
#include <userver/utils/boost_uuid7.hpp>

namespace {

using internview::services::JwtService;
using userver::server::handlers::Unauthorized;

const std::string kSecret = "unit-test-secret";

std::string MakeToken(const std::string& secret, const std::string& issuer,
                      std::chrono::system_clock::time_point expires_at) {
    return jwt::create()
        .set_type("JWT")
        .set_issuer(issuer)
        .set_issued_at(std::chrono::system_clock::now() - std::chrono::hours(2))
        .set_expires_at(expires_at)
        .set_payload_claim("user_id", jwt::claim(boost::uuids::to_string(
                                          userver::utils::generators::GenerateBoostUuidV7())))
        .set_payload_claim("role", jwt::claim(std::string{"intern"}))
        .set_payload_claim("password_version", jwt::claim(std::string{"0"}))
        .sign(jwt::algorithm::hs256{secret});
}

}  // namespace

TEST(JwtService, GeneratedTokenVerifiesWithSameClaims) {
    const JwtService service{kSecret};
    const auto user_id = userver::utils::generators::GenerateBoostUuidV7();

    const auto token = service.GenerateToken(user_id, "recruiter", 3);
    const auto parsed = service.VerifyToken(token);

    EXPECT_EQ(parsed.user_id, user_id);
    EXPECT_EQ(parsed.role, "recruiter");
    EXPECT_EQ(parsed.password_version, 3);
}

TEST(JwtService, RejectsTokenSignedWithAnotherSecret) {
    const JwtService issuer{"another-secret"};
    const JwtService verifier{kSecret};
    const auto token =
        issuer.GenerateToken(userver::utils::generators::GenerateBoostUuidV7(), "intern", 0);

    EXPECT_THROW(verifier.VerifyToken(token), Unauthorized);
}

TEST(JwtService, RejectsTokenWithForeignIssuer) {
    const JwtService service{kSecret};
    const auto token = MakeToken(kSecret, "not-internview",
                                 std::chrono::system_clock::now() + std::chrono::hours(1));

    EXPECT_THROW(service.VerifyToken(token), Unauthorized);
}

TEST(JwtService, RejectsExpiredToken) {
    const JwtService service{kSecret};
    const auto token =
        MakeToken(kSecret, "internview", std::chrono::system_clock::now() - std::chrono::hours(1));

    EXPECT_THROW(service.VerifyToken(token), Unauthorized);
}

TEST(JwtService, RejectsUnsignedToken) {
    const JwtService service{kSecret};
    const auto token = jwt::create()
                           .set_issuer("internview")
                           .set_payload_claim("user_id", jwt::claim(std::string{"x"}))
                           .sign(jwt::algorithm::none{});

    EXPECT_THROW(service.VerifyToken(token), Unauthorized);
}

TEST(JwtService, RejectsMalformedToken) {
    const JwtService service{kSecret};

    EXPECT_THROW(service.VerifyToken("definitely-not-a-jwt"), Unauthorized);
    EXPECT_THROW(service.VerifyToken("a.b.c"), Unauthorized);
}

TEST(JwtService, RejectsSignedTokenWithoutRequiredClaims) {
    const JwtService service{kSecret};
    const auto token = jwt::create()
                           .set_type("JWT")
                           .set_issuer("internview")
                           .set_issued_now()
                           .set_expires_in(std::chrono::minutes(5))
                           .sign(jwt::algorithm::hs256{kSecret});

    EXPECT_THROW(service.VerifyToken(token), Unauthorized);
}

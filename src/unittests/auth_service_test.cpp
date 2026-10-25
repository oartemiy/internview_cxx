#include "services/auth_service.hpp"

#include <sodium.h>

#include <memory>
#include <userver/crypto/hash.hpp>
#include <userver/engine/task/current_task.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/utest/utest.hpp>
#include <userver/utils/boost_uuid7.hpp>

#include "unittests/mocks/mock_refresh_token_repository.hpp"
#include "unittests/mocks/mock_user_repository.hpp"
#include "utils/exceptions.hpp"
#include "utils/password.hpp"

namespace {

using internview::mocks::MockRefreshTokenRepository;
using internview::mocks::MockUserRepository;
using internview::services::AuthService;
using internview::utils::Forbidden;
using userver::server::handlers::ConflictError;
using userver::server::handlers::ResourceNotFound;
using userver::server::handlers::Unauthorized;
using userver::utils::generators::GenerateBoostUuidV7;

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::StrictMock;
using ::testing::Throw;

internview::models::User MakeUser(const std::string& password, const std::string& role = "intern",
                                  int password_version = 0) {
    internview::models::User user;
    user.id = GenerateBoostUuidV7();
    user.login = "alice";
    user.password_hash = internview::utils::HashPassword(password);
    user.name = "Alice";
    user.role = role;
    user.created_at = std::chrono::system_clock::now();
    user.password_version = password_version;
    return user;
}

std::string Bearer(const std::string& token) {
    return "Bearer " + token;
}

class AuthServiceTest : public ::testing::Test {
public:
    static void SetUpTestSuite() {
        ASSERT_GE(sodium_init(), 0);
    }

protected:
    std::shared_ptr<StrictMock<MockUserRepository>> user_repository_ =
        std::make_shared<StrictMock<MockUserRepository>>();
    std::shared_ptr<StrictMock<MockRefreshTokenRepository>> refresh_repository_ =
        std::make_shared<StrictMock<MockRefreshTokenRepository>>();
    AuthService service_{"unit-test-secret", refresh_repository_, user_repository_,
                         userver::engine::current_task::GetTaskProcessor()};
};

}  // namespace

// ---------------------------------------------------------------- CheckAuthorization

UTEST_F(AuthServiceTest, CheckAuthorizationRejectsMissingHeader) {
    EXPECT_THROW(service_.CheckAuthorization(""), Unauthorized);
}

UTEST_F(AuthServiceTest, CheckAuthorizationRejectsNonBearerScheme) {
    EXPECT_THROW(service_.CheckAuthorization("Basic YWxpY2U6c2VjcmV0"), Unauthorized);
}

UTEST_F(AuthServiceTest, CheckAuthorizationRejectsEmptyBearerToken) {
    EXPECT_THROW(service_.CheckAuthorization("Bearer "), Unauthorized);
}

UTEST_F(AuthServiceTest, CheckAuthorizationAcceptsValidToken) {
    const auto user_id = GenerateBoostUuidV7();
    const auto token = service_.GenerateAccessToken(user_id, "recruiter", 2);

    const auto result = service_.CheckAuthorization(Bearer(token));

    EXPECT_EQ(result.user_id, user_id);
    EXPECT_EQ(result.role, "recruiter");
    EXPECT_EQ(result.access_token, token);
}

UTEST_F(AuthServiceTest, CheckAuthorizationRejectsTokenIssuedBeforePasswordChange) {
    const auto user_id = GenerateBoostUuidV7();
    const auto old_token = service_.GenerateAccessToken(user_id, "intern", 0);
    const auto new_token = service_.GenerateAccessToken(user_id, "intern", 1);

    service_.MarkAsChanged(user_id, 1, "intern");

    EXPECT_THROW(service_.CheckAuthorization(Bearer(old_token)), Unauthorized);
    EXPECT_EQ(service_.CheckAuthorization(Bearer(new_token)).user_id, user_id);
}

UTEST_F(AuthServiceTest, PasswordChangeOfOneUserDoesNotAffectOthers) {
    const auto alice = GenerateBoostUuidV7();
    const auto bob = GenerateBoostUuidV7();
    const auto bob_token = service_.GenerateAccessToken(bob, "intern", 0);

    service_.MarkAsChanged(alice, 1, "intern");

    EXPECT_EQ(service_.CheckAuthorization(Bearer(bob_token)).user_id, bob);
}

// ---------------------------------------------------------------- Login

UTEST_F(AuthServiceTest, LoginReturnsTokensAndStoresOnlyRefreshTokenHash) {
    const auto user = MakeUser("secret");
    std::string stored_hash;
    EXPECT_CALL(*user_repository_, GetUserByLogin("alice")).WillOnce(Return(user));
    EXPECT_CALL(*refresh_repository_, Create(user.id, _)).WillOnce(SaveArg<1>(&stored_hash));

    const auto response = service_.Login({"alice", "secret"});

    EXPECT_EQ(response.id, user.id);
    EXPECT_EQ(response.login, "alice");
    EXPECT_EQ(response.role, "intern");
    ASSERT_TRUE(response.access_token.has_value());
    ASSERT_TRUE(response.refresh_token.has_value());
    EXPECT_EQ(stored_hash, userver::crypto::hash::Sha256(*response.refresh_token));
    EXPECT_NE(stored_hash, *response.refresh_token);
    EXPECT_EQ(service_.CheckAuthorization(Bearer(*response.access_token)).user_id, user.id);
}

UTEST_F(AuthServiceTest, LoginWithWrongPasswordIssuesNoTokens) {
    EXPECT_CALL(*user_repository_, GetUserByLogin("alice")).WillOnce(Return(MakeUser("secret")));
    // refresh_repository_->Create must not be called (StrictMock)

    EXPECT_THROW(service_.Login({"alice", "wrong"}), Unauthorized);
}

UTEST_F(AuthServiceTest, LoginOfUnknownUserLooksLikeWrongPassword) {
    EXPECT_CALL(*user_repository_, GetUserByLogin("ghost"))
        .WillOnce(Throw(ResourceNotFound{userver::formats::json::MakeObject("message", "nf")}));

    EXPECT_THROW(service_.Login({"ghost", "secret"}), Unauthorized);
}

UTEST_F(AuthServiceTest, LoginTokenCarriesPasswordVersionFromRepository) {
    const auto user = MakeUser("secret", "intern", 5);
    EXPECT_CALL(*user_repository_, GetUserByLogin("alice")).WillOnce(Return(user));
    EXPECT_CALL(*refresh_repository_, Create(user.id, _));

    const auto response = service_.Login({"alice", "secret"});
    service_.MarkAsChanged(user.id, 5, "intern");

    EXPECT_NO_THROW(service_.CheckAuthorization(Bearer(*response.access_token)));
}

// ---------------------------------------------------------------- Register

UTEST_F(AuthServiceTest, RegisterHashesPasswordAndIssuesTokens) {
    internview::dto::user::CreateDTO created;
    EXPECT_CALL(*user_repository_, CreateUser(_))
        .WillOnce(DoAll(SaveArg<0>(&created), [](const internview::dto::user::CreateDTO& dto) {
            return internview::dto::user::ResponseDTO{
                dto.id,          dto.login,       dto.name,
                dto.role,        dto.description, dto.profile_pic,
                std::chrono::system_clock::now(), {}, {}};
        }));
    EXPECT_CALL(*refresh_repository_, Create(_, _));

    const auto response =
        service_.Register({"alice", "secret", "Alice", "recruiter", "about", std::nullopt});

    EXPECT_EQ(created.login, "alice");
    EXPECT_EQ(created.role, "recruiter");
    EXPECT_NE(created.password_hash, "secret");
    EXPECT_TRUE(internview::utils::VerifyPassword("secret", created.password_hash));
    EXPECT_EQ(response.id, created.id);
    ASSERT_TRUE(response.access_token.has_value());
    ASSERT_TRUE(response.refresh_token.has_value());

    const auto auth = service_.CheckAuthorization(Bearer(*response.access_token));
    EXPECT_EQ(auth.user_id, created.id);
    EXPECT_EQ(auth.role, "recruiter");
}

UTEST_F(AuthServiceTest, RegisterWithTakenLoginIssuesNoTokens) {
    EXPECT_CALL(*user_repository_, CreateUser(_))
        .WillOnce(Throw(ConflictError{userver::formats::json::MakeObject("message", "taken")}));
    // refresh_repository_->Create must not be called (StrictMock)

    EXPECT_THROW(service_.Register({"alice", "secret", "Alice", "intern", {}, {}}), ConflictError);
}

// ---------------------------------------------------------------- ChangePassword

UTEST_F(AuthServiceTest, ChangePasswordWithWrongOldPasswordChangesNothing) {
    const auto user = MakeUser("old-secret");
    EXPECT_CALL(*user_repository_, GetUserById(user.id)).WillOnce(Return(user));
    // UpdatePasswordHash / RevokeAllUserTokens must not be called (StrictMock)

    EXPECT_THROW(service_.ChangePassword({user.id, "wrong", "new-secret"}), Forbidden);
}

// ---------------------------------------------------------------- CheckPassword

UTEST_F(AuthServiceTest, CheckPasswordAcceptsCorrectPassword) {
    const auto user = MakeUser("secret");
    EXPECT_CALL(*user_repository_, GetUserById(user.id)).WillOnce(Return(user));

    EXPECT_NO_THROW(service_.CheckPassword(user.id, "secret"));
}

UTEST_F(AuthServiceTest, CheckPasswordRejectsWrongPassword) {
    const auto user = MakeUser("secret");
    EXPECT_CALL(*user_repository_, GetUserById(user.id)).WillOnce(Return(user));

    EXPECT_THROW(service_.CheckPassword(user.id, "wrong"), Forbidden);
}

UTEST_F(AuthServiceTest, ChangePasswordStoresNewHashRevokesSessionsAndOldAccessTokens) {
    const auto user = MakeUser("old-secret");
    const auto old_token = service_.GenerateAccessToken(user.id, user.role, 0);
    std::string new_hash;
    EXPECT_CALL(*user_repository_, GetUserById(user.id)).WillOnce(Return(user));
    EXPECT_CALL(*user_repository_, UpdatePasswordHash(user.id, _)).WillOnce(SaveArg<1>(&new_hash));
    EXPECT_CALL(*refresh_repository_, RevokeAllUserTokens(user.id));

    service_.ChangePassword({user.id, "old-secret", "new-secret"});

    EXPECT_TRUE(internview::utils::VerifyPassword("new-secret", new_hash));
    EXPECT_FALSE(internview::utils::VerifyPassword("old-secret", new_hash));
    EXPECT_THROW(service_.CheckAuthorization(Bearer(old_token)), Unauthorized);
    const auto new_token = service_.GenerateAccessToken(user.id, user.role, 1);
    EXPECT_NO_THROW(service_.CheckAuthorization(Bearer(new_token)));
}

// ---------------------------------------------------------------- Refresh / Revoke

UTEST_F(AuthServiceTest, RefreshReturnsRotatedRefreshTokenAndNewAccessToken) {
    const auto user = MakeUser("secret", "recruiter", 4);
    EXPECT_CALL(*refresh_repository_, RefreshToken("old-refresh"))
        .WillOnce(Return(internview::repositories::interfaces::IRefreshTokenRepository::NewRefreshToken{
            "new-refresh", user.id}));
    EXPECT_CALL(*user_repository_, GetUserById(user.id)).WillOnce(Return(user));

    const auto tokens = service_.Refresh("old-refresh");

    EXPECT_EQ(tokens.refresh_token, "new-refresh");
    EXPECT_NE(tokens.access_token, "new-refresh");
    service_.MarkAsChanged(user.id, 4, user.role);
    EXPECT_NO_THROW({
        const auto auth = service_.CheckAuthorization(Bearer(tokens.access_token));
        EXPECT_EQ(auth.user_id, user.id);
        EXPECT_EQ(auth.role, "recruiter");
    });
}

UTEST_F(AuthServiceTest, RefreshWithInvalidTokenPropagatesUnauthorized) {
    EXPECT_CALL(*refresh_repository_, RefreshToken("bad"))
        .WillOnce(Throw(Unauthorized{userver::formats::json::MakeObject("message", "bad")}));
    // user_repository_->GetUserById must not be called (StrictMock)

    EXPECT_THROW(service_.Refresh("bad"), Unauthorized);
}

UTEST_F(AuthServiceTest, RevokeRevokesAllUserTokens) {
    const auto user_id = GenerateBoostUuidV7();
    EXPECT_CALL(*refresh_repository_, RevokeAllUserTokens(user_id));

    service_.Revoke(user_id);
}

UTEST_F(AuthServiceTest, GenerateRefreshTokenIsRandomAndStoredHashed) {
    const auto user_id = GenerateBoostUuidV7();
    std::vector<std::string> hashes;
    EXPECT_CALL(*refresh_repository_, Create(user_id, _))
        .Times(2)
        .WillRepeatedly([&hashes](const auto&, const std::string& hash) {
            hashes.push_back(hash);
        });

    const auto first = service_.GenerateRefreshToken(user_id);
    const auto second = service_.GenerateRefreshToken(user_id);

    EXPECT_NE(first, second);
    EXPECT_GE(first.size(), 40U);  // 32 random bytes in base64
    ASSERT_EQ(hashes.size(), 2U);
    EXPECT_EQ(hashes[0], userver::crypto::hash::Sha256(first));
    EXPECT_EQ(hashes[1], userver::crypto::hash::Sha256(second));
}

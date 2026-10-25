#include "utils/password.hpp"

#include <sodium.h>

#include <userver/utest/utest.hpp>

namespace {

class PasswordTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ASSERT_GE(sodium_init(), 0);
    }
};

}  // namespace

using internview::utils::HashPassword;
using internview::utils::VerifyPassword;

TEST_F(PasswordTest, HashIsArgon2AndNotPlaintext) {
    const auto hash = HashPassword("qwerty123");

    ASSERT_FALSE(hash.empty());
    EXPECT_NE(hash, "qwerty123");
    EXPECT_TRUE(hash.starts_with("$argon2id$"));
}

TEST_F(PasswordTest, VerifyAcceptsCorrectPassword) {
    EXPECT_TRUE(VerifyPassword("qwerty123", HashPassword("qwerty123")));
}

TEST_F(PasswordTest, VerifyRejectsWrongPassword) {
    const auto hash = HashPassword("qwerty123");

    EXPECT_FALSE(VerifyPassword("qwerty124", hash));
    EXPECT_FALSE(VerifyPassword("", hash));
}

TEST_F(PasswordTest, SamePasswordGivesDifferentSaltedHashes) {
    const auto first = HashPassword("qwerty123");
    const auto second = HashPassword("qwerty123");

    EXPECT_NE(first, second);
    EXPECT_TRUE(VerifyPassword("qwerty123", first));
    EXPECT_TRUE(VerifyPassword("qwerty123", second));
}

TEST_F(PasswordTest, VerifyRejectsGarbageHash) {
    EXPECT_FALSE(VerifyPassword("qwerty123", "not-a-hash"));
    EXPECT_FALSE(VerifyPassword("qwerty123", ""));
}

TEST_F(PasswordTest, SupportsUnicodePasswords) {
    const auto hash = HashPassword("пароль-密码");

    EXPECT_TRUE(VerifyPassword("пароль-密码", hash));
    EXPECT_FALSE(VerifyPassword("пароль", hash));
}

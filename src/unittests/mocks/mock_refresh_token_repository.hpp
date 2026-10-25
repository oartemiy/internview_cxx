#pragma once

#include <gmock/gmock.h>

#include "repositories/interfaces/refresh_token_repository.hpp"

namespace internview::mocks {

class MockRefreshTokenRepository : public repositories::interfaces::IRefreshTokenRepository {
public:
    MOCK_METHOD(void, Create, (const boost::uuids::uuid& user_id, const std::string& token_hash),
                (override));
    MOCK_METHOD(std::optional<models::RefreshToken>, GetByTokenHash,
                (const std::string& token_hash), (override));
    MOCK_METHOD(void, Revoke, (const boost::uuids::uuid& id), (override));
    MOCK_METHOD(void, RevokeAllUserTokens, (const boost::uuids::uuid& user_id), (override));
    MOCK_METHOD(void, DeleteToken, (const boost::uuids::uuid& id), (override));
    MOCK_METHOD(NewRefreshToken, RefreshToken, (const std::string& refresh_token), (override));
    MOCK_METHOD(void, ClearExpiredTokens, (), (override));
};

}  // namespace internview::mocks

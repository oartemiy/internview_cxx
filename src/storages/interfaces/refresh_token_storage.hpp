#pragma once

#include "boost/uuid/uuid.hpp"
#include "models/refresh_token.hpp"

namespace internview::storages::interfaces {

class IRefreshTokenStorage {
public:
    struct NewRefreshToken {
        std::string token;
        boost::uuids::uuid user_id;
    };

    virtual ~IRefreshTokenStorage() = default;

    virtual void Create(const boost::uuids::uuid& user_id, const std::string& token_hash) = 0;

    virtual std::optional<models::RefreshToken> GetByTokenHash(const std::string& token_hash) = 0;

    virtual void Revoke(const boost::uuids::uuid& id) = 0;

    virtual void RevokeAllUserTokens(const boost::uuids::uuid& user_id) = 0;

    virtual void DeleteToken(const boost::uuids::uuid& id) = 0;

    virtual NewRefreshToken RefreshToken(const std::string& refresh_token) = 0;

    virtual void ClearExpiredTokens() = 0;
};

}  // namespace internview::storages::interfaces

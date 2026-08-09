#pragma once

#include <optional>
#include <userver/storages/postgres/cluster.hpp>

#include "dto/user_dto.hpp"
#include "models/user.hpp"
#include "services/jwt_service.hpp"
#include "userver/components/component_context.hpp"

namespace internview::storages {

using internview::models::User;

class UserStorage {
public:
    explicit UserStorage(const userver::components::ComponentContext& component_context);

    // NOTE: mostly always (100%) without errors (UUIDv7 repeat probability -> 0.0)
    std::optional<dto::user::ResponseDTO> CreateUser(
        const internview::dto::user::CreateDTO& dto) const;

    std::optional<dto::user::ResponseDTO> UpdateUser(
        const std::string& token, const internview::dto::user::UpdateDTO& dto) const;

    bool DeleteUser(const std::string& token, const internview::dto::user::DeleteDTO& dto) const;

    std::optional<dto::user::ResponseDTO> GetUserByJWT(const std::string& token) const;

    std::optional<dto::user::ResponseDTO> LoginUser(
        const internview::dto::user::LoginDTO& dto) const;

    bool ChangeUserPassword(const std::string& token,
                            const dto::user::ChangePasswordDTO& dto) const;

private:
    std::optional<User> GetUserById(const boost::uuids::uuid& id) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
    internview::services::JwtService jwt_service_;
};

}  // namespace internview::storages

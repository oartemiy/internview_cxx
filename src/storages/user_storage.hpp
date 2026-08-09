#pragma once

#include <userver/storages/postgres/cluster.hpp>
#include "models/user.hpp"
#include "services/jwt_service.hpp"
#include "userver/components/component_context.hpp"
#include "dto/user_dto.hpp"

namespace internview::storages {

using internview::models::User;

class UserStorage {
public:
    explicit UserStorage(const userver::components::ComponentContext& component_context);

    // NOTE: mostly always (100%) without errors
    dto::user::ResponseDTO CreateUser(const internview::dto::user::CreateDTO& dto) const;

    // void UpdateUser(const internview::dto::user::UpdateDTO& dto) const;

    // void DeleteUser(const internview::dto::user::)

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    internview::services::JwtService jwt_service_;
};

}  // namespace internview::storages

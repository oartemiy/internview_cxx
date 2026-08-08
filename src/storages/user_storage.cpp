#include "user_storage.hpp"

#include <chrono>

#include "dto/user_dto.hpp"
#include "user_storage_queries/sql_queries.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include "utils/password.hpp"

namespace internview::storages {

UserStorage::UserStorage(const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()) {
}

dto::user::ResponseDTO UserStorage::CreateUser(const internview::dto::user::CreateDTO& dto) const {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    auto password_hash = internview::utils::password::HashPassword(dto.password);
    auto pg_res =
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                             user_storage_queries::sql::kCreateUser, id, dto.login, password_hash,
                             dto.name, dto.role, dto.description, dto.profile_pic);
    dto::user::ResponseDTO ressult;
    ressult.id = id;
    ressult.login = dto.login;
    ressult.created_at = pg_res[0][0].As<std::chrono::system_clock::time_point>();
    ressult.name = dto.name;
    ressult.role = dto.role;
    ressult.profile_pic = dto.profile_pic;
    ressult.description = dto.description;
    return ressult;
}

}  // namespace internview::storages

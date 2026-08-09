#include "user_storage.hpp"

#include <sodium.h>

#include <chrono>
#include <optional>
#include <string>

#include "dto/user_dto.hpp"
#include "models/user.hpp"
#include "user_storage_queries/sql_queries.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/storages/secdist/provider_component.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include "utils/password.hpp"

namespace internview::storages {

std::optional<models::User> UserStorage::GetUserById(const boost::uuids::uuid& id) const {
    auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                    user_storage_queries::sql::kGetUserById, id);
    if (res.IsEmpty()) {
        return std::nullopt;
    }
    auto user = res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);
    return user;
}

UserStorage::UserStorage(const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      jwt_service_(component_context
                       .FindComponent<userver::components::DefaultSecdistProvider>(
                           "default-secdist-provider")
                       .Get()
                       .As<userver::formats::json::Value>()["jwt_secret"]
                       .As<std::string>()) {
    // !NOTE: For password verifing and hashing
    if (sodium_init() != 0) {
        throw std::runtime_error{"Sodium init error"};
    }
}

std::optional<dto::user::ResponseDTO> UserStorage::CreateUser(
    const internview::dto::user::CreateDTO& dto) const {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    auto password_hash = internview::utils::password::HashPassword(dto.password);
    auto pg_res =
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                             user_storage_queries::sql::kCreateUser, id, dto.login, password_hash,
                             dto.name, dto.role, dto.description, dto.profile_pic);
    dto::user::ResponseDTO ressult;
    if (pg_res.IsEmpty()) {
        return std::nullopt;
    }
    ressult.id = id;
    ressult.login = dto.login;
    ressult.created_at = pg_res[0][0].As<std::chrono::system_clock::time_point>();
    ressult.name = dto.name;
    ressult.role = dto.role;
    ressult.profile_pic = dto.profile_pic;
    ressult.description = dto.description;
    ressult.token = jwt_service_.GenerateToken(id, dto.role);
    return ressult;
}

bool UserStorage::DeleteUser(const std::string& token,
                             const internview::dto::user::DeleteDTO& dto) const {
    auto id = jwt_service_.VerifyToken(token);
    if (id->is_nil()) {
        return false;
    }
    auto user = GetUserById(*id);
    if (!user || !internview::utils::password::VerifyPassword(dto.password, user->password_hash)) {
        return false;
    }
    auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                    user_storage_queries::sql::kDeleteUser, *id);
    if (res.IsEmpty()) {
        return false;
    }
    return true;
}

}  // namespace internview::storages

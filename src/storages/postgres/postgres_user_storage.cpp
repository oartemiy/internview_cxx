#include "postgres_user_storage.hpp"

#include <sodium.h>

#include <cctype>
#include <optional>
#include <stdexcept>
#include <string>
#include <userver/server/handlers/exceptions.hpp>

#include "components/storages/img_storage_component.hpp"
#include "dto/user_dto.hpp"
#include "models/user.hpp"
#include "user_storage_queries/sql_queries.hpp"
#include "userver/engine/async.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/exceptions.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "utils/password.hpp"

namespace internview::storages::postgres {

PostgresUserStorage::PostgresUserStorage(
    [[maybe_unused]] const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : crypto_tp_(component_context.GetTaskProcessor("crypt-task-processor")),
      pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      img_storage_(component_context.FindComponent<internview::components::ImgStorageComponent>()
                       .GetStorage()) {
    // !NOTE: For password verifing and hashing
    if (sodium_init() != 0) {
        throw std::runtime_error{"Sodium init error"};
    }
}

internview::dto::user::ResponseDTO PostgresUserStorage::CreateUser(
    const dto::user::CreateDTO& dto) {
    try {
        auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                           user_storage_queries::sql::kCreateUser, dto.id,
                                           dto.login, dto.password_hash, dto.name, dto.role,
                                           dto.description, dto.profile_pic);

        return dto::user::ResponseDTO{dto.id,
                                      dto.login,
                                      dto.name,
                                      dto.role,
                                      dto.description,
                                      dto.profile_pic,
                                      pg_res[0][0].As<std::chrono::system_clock::time_point>(),
                                      {},
                                      {}};
    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Login: " + dto.login + " is taken. Try another one"));
    }
}

models::User PostgresUserStorage::GetUserById(const boost::uuids::uuid& id) {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                       user_storage_queries::sql::kGetUserById, id);

    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ResourceNotFound(userver::formats::json::MakeObject(
            "message", "User with id: " + boost::uuids::to_string(id) + " not found"));
    }
    auto user = pg_res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);
    return user;
}

internview::models::User PostgresUserStorage::GetUserByLogin(const std::string& login) {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                       user_storage_queries::sql::kGetUserByLogin, login);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ResourceNotFound(
            userver::formats::json::MakeObject("message", "User with id: " + login + " not found"));
    }
    auto user = pg_res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);
    return user;
}

void PostgresUserStorage::UpdatePasswordHash(const boost::uuids::uuid& id,
                                             const std::string& new_password_hash) {
    auto pg_res =
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                             user_storage_queries::sql::kChangeUserPassword, id, new_password_hash);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "User with id: " + boost::uuids::to_string(id) + " not found"));
    }
}

dto::user::ResponseDTO PostgresUserStorage::UpdateUser(
    const internview::dto::user::UpdateDTO& dto) {
    if (!dto.has_description_in_request && !dto.has_login_in_request && !dto.has_name_in_request &&
        !dto.has_profile_pic_in_request) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "Empty request data body. Nothing to update"));
    }
    auto model = GetUserById(dto.id);
    auto old_profile_pic = model.profile_pic;
    int count_changes = 0;
    if (dto.has_login_in_request && model.login != dto.login) {
        model.login = dto.login;
        ++count_changes;
    }
    if (dto.has_name_in_request && model.name != dto.name) {
        model.name = dto.name;
        ++count_changes;
    }
    if (dto.has_description_in_request && model.description != dto.description) {
        model.description = dto.description;
        ++count_changes;
    }
    if (dto.has_profile_pic_in_request && model.profile_pic != dto.profile_pic) {
        model.profile_pic = dto.profile_pic;
        ++count_changes;
    }
    if (count_changes == 0) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Nothing to update"));
    }
    if (model.login.length() <= 1) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Login must be at least 2 chars"));
    }
    try {
        auto pg_res =
            pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                 user_storage_queries::sql::kUpdateUser, model.login, model.name,
                                 model.description, model.profile_pic, dto.id);
        if (old_profile_pic != std::nullopt && model.profile_pic == std::nullopt) {
            img_storage_->Delete(*old_profile_pic);
        }
        auto resp_dto =
            dto::user::ResponseDTO{dto.id,           model.login,       model.name,
                                   model.role,       model.description, model.profile_pic,
                                   model.created_at, std::nullopt};
        return resp_dto;
    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Login: " + model.login + " has already taken"));
    }
}

void PostgresUserStorage::DeleteUser(const internview::dto::user::DeleteDTO& dto) {
    auto user = GetUserById(dto.id);

    auto verify_res_fut = userver::engine::AsyncNoTracing(crypto_tp_, [&dto, &user] {
        return internview::utils::VerifyPassword(dto.password, user.password_hash);
    });

    auto verify_res = verify_res_fut.Get();
    if (!verify_res) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Password is incorrect"));
    }

    auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                    user_storage_queries::sql::kDeleteUser, dto.id);
    if (user.profile_pic != std::nullopt) {
        img_storage_->Delete(*user.profile_pic);
    }
    if (res.IsEmpty()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "User with login: " + dto.login + " not found"));
    }
}

void PostgresUserStorage::UploadProfilePic(const boost::uuids::uuid& id,
                                           const utils::File& file_arg) {

    auto user = GetUserById(id);
    if (user.profile_pic != std::nullopt) {
        img_storage_->Delete(*user.profile_pic);
    }
    auto key = img_storage_->Save(file_arg);
    auto update_dto =
        dto::user::UpdateDTO{false, false, false, true, id, "", "", std::nullopt, key};
    auto res = UpdateUser(update_dto);
}

std::optional<std::pair<std::string, std::string>> PostgresUserStorage::GetProfilePic(
    const boost::uuids::uuid& id) {

    auto user = GetUserById(id);

    auto pic = user.profile_pic;
    if (!pic) {
        return std::nullopt;
    }
    try {
        // TODO: impl later
        auto file = img_storage_->Load(*pic);
        std::pair<std::string, std::string> res{*pic, std::move(file)};
        return res;
    } catch (std::runtime_error& e) {
        return std::nullopt;
    }
}

}  // namespace internview::storages::postgres

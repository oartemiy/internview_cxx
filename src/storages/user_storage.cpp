#include "user_storage.hpp"

#include <sodium.h>

#include <cctype>
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>

#include "dto/user_dto.hpp"
#include "errors/errors.hpp"
#include "models/user.hpp"
#include "services/file_service.hpp"
#include "user_storage_queries/sql_queries.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/logging/log.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include "userver/utils/uuid4.hpp"
#include "utils/password.hpp"

namespace internview::storages {

UserStorage::UserStorage(std::shared_ptr<services::AuthService> auth_service_ptr,
                         const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      auth_service_ptr_(auth_service_ptr),
      file_service_(config, component_context) {
    // !NOTE: For password verifing and hashing
    if (sodium_init() != 0) {
        throw std::runtime_error{"Sodium init error"};
    }
}

models::User UserStorage::GetUserById(const boost::uuids::uuid& id) const {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       user_storage_queries::sql::kGetUserById, id);

    if (pg_res.IsEmpty()) {
        throw errors::NotFoundError("User not found");
    }
    auto user = pg_res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);
    return user;
}

dto::user::ResponseDTO UserStorage::CreateUser(const internview::dto::user::CreateDTO& dto) const {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    auto password_hash = internview::utils::HashPassword(dto.password);
    auto pg_res =
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                             user_storage_queries::sql::kCreateUser, id, dto.login, password_hash,
                             dto.name, dto.role, dto.description, dto.profile_pic);

    if (pg_res.IsEmpty()) {
        throw internview::errors::ConflictError("Login " + dto.login + " is taken. Try another one");
    }
    auto resp_dto = dto::user::ResponseDTO{id,
                                           dto.login,
                                           dto.name,
                                           dto.role,
                                           dto.description,
                                           dto.profile_pic,
                                           pg_res[0][0].As<std::chrono::system_clock::time_point>(),
                                           auth_service_ptr_->GenerateJwtToken(id, dto.role)};
    return resp_dto;
}

dto::user::ResponseDTO UserStorage::UpdateUser(const internview::dto::user::UpdateDTO& dto) const {

    auto user = GetUserById(dto.user_id);

    auto login = dto.login ? *dto.login : user.login;
    auto name = dto.name ? *dto.name : user.name;
    auto description = dto.description ? *dto.description : user.description;
    auto profile_pic = dto.profile_pic ? *dto.profile_pic : user.profile_pic;
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       user_storage_queries::sql::kUpdateUser, login, name,
                                       description, profile_pic, dto.user_id);
    if (pg_res.IsEmpty()) {
        throw errors::ConflictError("Login " + login + " has already taken");
    }
    auto resp_dto = dto::user::ResponseDTO{
        dto.user_id, login, name, user.role, description, profile_pic, user.created_at, ""};
    return resp_dto;
}

void UserStorage::DeleteUser(const internview::dto::user::DeleteDTO& dto) const {
    auto user = GetUserById(dto.user_id);
    if (!internview::utils::VerifyPassword(dto.password, user.password_hash)) {
        throw errors::InvalidPasswordError("Password is incorrect");
    }
    auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                    user_storage_queries::sql::kDeleteUser, dto.user_id);
    if (res.IsEmpty()) {
        throw errors::NotFoundError("User not found");
    }
}

dto::user::ResponseDTO UserStorage::LoginUser(const internview::dto::user::LoginDTO& dto) const {

    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       user_storage_queries::sql::kLoginUser, dto.login);
    if (pg_res.IsEmpty()) {
        throw errors::NotFoundError("User not found");
    }
    auto user = pg_res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);
    if (!utils::VerifyPassword(dto.password, user.password_hash)) {
        throw errors::InvalidPasswordError("Password is incorrect");
    }
    auto token = auth_service_ptr_->GenerateJwtToken(user.id, user.role);
    auto resp_dto =
        dto::user::ResponseDTO{user.id,          user.login,       user.name,       user.role,
                               user.description, user.profile_pic, user.created_at, token};
    return resp_dto;
}

void UserStorage::ChangeUserPassword(const dto::user::ChangePasswordDTO& dto) const {
    auto user = GetUserById(dto.user_id);
    if (!utils::VerifyPassword(dto.old_password, user.password_hash)) {
        throw errors::InvalidPasswordError("Password is incorrect");
    }

    auto new_password_hash = internview::utils::HashPassword(dto.new_password);
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       user_storage_queries::sql::kChangeUserPassword, dto.user_id,
                                       new_password_hash);
    if (pg_res.IsEmpty()) {
        throw errors::NotFoundError("User not found");
    }
}

void UserStorage::UploadProfilePic(const boost::uuids::uuid& user_id,
                                   const userver::server::http::FormDataArg& file_arg) {
    auto ext =
        std::filesystem::path(file_arg.filename ? *file_arg.filename : "").extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".jpg" && ext != ".png" && ext != ".jpeg") {
        throw errors::FileUploadError("Invalid file format");
    }
    auto new_uuid = userver::utils::generators::GenerateUuid();
    auto full_path = file_service_.img_folder + new_uuid + ext;
    if (file_service_.WriteFile(full_path, file_arg.value)) {
        // TODO: use server path instead
        auto server_path = new_uuid + ext;
        auto update_dto =
            dto::user::UpdateDTO{user_id, std::nullopt, std::nullopt, std::nullopt, server_path};
        auto res = UpdateUser(update_dto);
    } else {
        throw errors::FileUploadError("Something went wrong");
    }
}

std::optional<std::pair<std::string, std::string>> UserStorage::GetProfilePic(
    const boost::uuids::uuid& user_id) {

    auto user = GetUserById(user_id);

    auto pic = user.profile_pic;
    if (!pic) {
        return std::nullopt;
    }
    LOG_INFO() << *pic;
    auto file = file_service_.ReadFile(services::FileService::img_folder + *pic);
    if (!file) {
        return std::nullopt;
    }
    std::pair<std::string, std::string> res{*pic, *file};
    return res;
}

}  // namespace internview::storages

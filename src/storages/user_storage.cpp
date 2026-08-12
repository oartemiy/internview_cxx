#include "user_storage.hpp"

#include <sodium.h>

#include <cctype>
#include <chrono>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>

#include "dto/user_dto.hpp"
#include "models/user.hpp"
#include "services/file_service.hpp"
#include "user_storage_queries/sql_queries.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/logging/log.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/storages/secdist/provider_component.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include "userver/utils/uuid4.hpp"
#include "utils/password.hpp"

namespace internview::storages {

std::optional<models::User> UserStorage::GetUserById(const boost::uuids::uuid& id) const {
    auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                    user_storage_queries::sql::kGetUserById, id);
    std::optional<models::User> user;
    if (res.IsEmpty()) {
        return user;
    }
    user.emplace(res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag));
    return user;
}

UserStorage::UserStorage(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      jwt_service_(component_context
                       .FindComponent<userver::components::DefaultSecdistProvider>(
                           "default-secdist-provider")
                       .Get()
                       .As<userver::formats::json::Value>()["jwt_secret"]
                       .As<std::string>()),
      file_service_(config, component_context) {
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
    std::optional<dto::user::ResponseDTO> response;
    if (pg_res.IsEmpty()) {
        return response;
    }
    response.emplace(id, dto.login, dto.name, dto.role, dto.description, dto.profile_pic,
                     pg_res[0][0].As<std::chrono::system_clock::time_point>(),
                     jwt_service_.GenerateToken(id, dto.role));
    return response;
}

bool UserStorage::DeleteUser(const std::string& token,
                             const internview::dto::user::DeleteDTO& dto) const {
    auto id = jwt_service_.VerifyToken(token);
    if (!id) {
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

std::optional<dto::user::ResponseDTO> UserStorage::UpdateUser(
    const std::string& token, const internview::dto::user::UpdateDTO& dto) const {
    auto id = jwt_service_.VerifyToken(token);
    std::optional<dto::user::ResponseDTO> response;
    if (!id) {
        return response;
    }
    auto user = GetUserById(*id);
    if (!user) {
        return response;
    }
    auto login = dto.login ? *dto.login : user->login;
    auto name = dto.name ? *dto.name : user->name;
    auto description = dto.description ? *dto.description : user->description;
    auto profile_pic = dto.profile_pic ? *dto.profile_pic : user->profile_pic;
    auto res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                    user_storage_queries::sql::kUpdateUser, login, name,
                                    description, profile_pic, *id);
    if (res.IsEmpty()) {
        return response;
    }
    response.emplace(*id, login, name, user->role, description, profile_pic, user->created_at,
                     token);
    return response;
}

std::optional<dto::user::ResponseDTO> UserStorage::GetUserByJWT(const std::string& token) const {
    auto id = jwt_service_.VerifyToken(token);
    std::optional<dto::user::ResponseDTO> response;
    if (!id) {
        return response;
    }
    auto user = GetUserById(*id);
    if (!user) {
        return response;
    }
    response.emplace(user->id, user->login, user->name, user->role, user->description,
                     user->profile_pic, user->created_at, token);
    return response;
}

std::optional<dto::user::ResponseDTO> UserStorage::LoginUser(
    const internview::dto::user::LoginDTO& dto) const {
    std::optional<dto::user::ResponseDTO> ressult;
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       user_storage_queries::sql::kLoginUser, dto.login);
    if (pg_res.IsEmpty()) {
        return ressult;
    }
    auto user = pg_res.AsSingleRow<internview::models::User>(userver::storages::postgres::kRowTag);
    if (!utils::password::VerifyPassword(dto.password, user.password_hash)) {
        return ressult;
    }
    auto token = jwt_service_.GenerateToken(user.id, user.role);
    ressult.emplace(user.id, user.login, user.name, user.role, user.description, user.profile_pic,
                    user.created_at, token);
    return ressult;
}

bool UserStorage::ChangeUserPassword(const std::string& token,
                                     const dto::user::ChangePasswordDTO& dto) const {
    auto id = jwt_service_.VerifyToken(token);
    if (!id) {
        return false;
    }
    auto user = GetUserById(*id);
    if (!utils::password::VerifyPassword(dto.old_password, user->password_hash)) {
        return false;
    }

    auto new_password_hash = internview::utils::password::HashPassword(dto.new_password);
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       user_storage_queries::sql::kChangeUserPassword, *id,
                                       new_password_hash);
    if (pg_res.IsEmpty()) {
        return false;
    }
    return true;
}

std::expected<std::string, UserStorage::UploadError> UserStorage::UploadProfilePic(
    const std::string& token, const userver::server::http::FormDataArg& file_arg) {
    auto id = jwt_service_.VerifyToken(token);
    if (!id) {
        return std::unexpected<UploadError>(UploadError::BadJWT);
    }

    auto ext =
        std::filesystem::path(file_arg.filename ? *file_arg.filename : "").extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".jpg" && ext != ".png" && ext != ".jpeg") {
        return std::unexpected<UploadError>(UploadError::InvalidFileType);
    }
    auto new_uuid = userver::utils::generators::GenerateUuid();
    auto full_path = file_service_.img_folder + new_uuid + ext;
    if (file_service_.WriteFile(full_path, file_arg.value)) {
        // TODO: use server path instead
        auto server_path = new_uuid + ext;
        auto update_dto =
            dto::user::UpdateDTO{std::nullopt, std::nullopt, std::nullopt, server_path};
        auto res = UpdateUser(token, update_dto);
        if (!res) {
            return std::unexpected<UploadError>(UploadError::UnknownError);
        }
        return full_path;
    } else {
        return std::unexpected<UploadError>(UploadError::UnknownError);
    }
}

std::optional<std::pair<std::string, std::string>> UserStorage::GetProfilePic(
    const std::string& token) {
    auto id = jwt_service_.VerifyToken(token);
    if (!id) {
        return std::nullopt;
    }
    auto user = GetUserById(*id);
    if (!user) {
        return std::nullopt;
    }
    auto pic = user->profile_pic;
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

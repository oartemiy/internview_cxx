#pragma once

#include <memory>
#include <string>
#include <userver/storages/postgres/cluster.hpp>

#include "dto/user_dto.hpp"
#include "models/user.hpp"
#include "services/auth_service.hpp"
#include "services/file_service.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/server/http/form_data_arg.hpp"

namespace internview::storages {

using internview::models::User;

class UserStorage {
public:
    explicit UserStorage(std::shared_ptr<services::AuthService> auth_service_ptr,
                         const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context);

    /**
     * @brief Create the User object
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throw internview::errors::ConflictError when login has already taken
     */
    dto::user::ResponseDTO CreateUser(const internview::dto::user::CreateDTO& dto) const;

    /**
     * @brief Update the User object
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throws internview::errors::ConflictError when login had already taken
               internview::errors::NotFoundError when user_id not found
     */
    dto::user::ResponseDTO UpdateUser(const internview::dto::user::UpdateDTO& dto) const;

    /**
     * @brief Delete the User object
     *
     * @param dto
     * @throws internview::errors::NotFoundError when user_id not found
               internview::errors::InvalidPasswordError when password is incorrect
     */
    void DeleteUser(const internview::dto::user::DeleteDTO& dto) const;

    /**
     * @brief Get the User By Id object
     *
     * @param id
     * @return User
     * @throw internview::errors::NotFoundError when User with user_id does not exists
     */
    User GetUserById(const boost::uuids::uuid& user_id) const;

    /**
     * @brief Login User
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throws  internview::errors::NotFoundError when user does not exists
                internview::errors::InvalidPasswordError when password is incorrect
     */
    dto::user::ResponseDTO LoginUser(const internview::dto::user::LoginDTO& dto) const;

    /**
     * @brief Change user's password
     *
     * @param dto
     * @throws internview::errors::NotFoundError when user does not exists
               internview::errors::InvalidPasswordError when password is incorrect
     */
    void ChangeUserPassword(const dto::user::ChangePasswordDTO& dto) const;

    /**
     * @brief Uploads and sets new user's profile picture
     *
     * @param file_arg
     * @throws internview::errors::NotFoundError when user does not exists
               internview::errors::FileUploadError when 
     */
    void UploadProfilePic(const boost::uuids::uuid& user_id,
                          const userver::server::http::FormDataArg& file_arg);

    /**
     * @brief Get the Profile Pic object
     *
     * @param token
     * @return std::pair<std::string, std::string> 'filename 'file_data
     * @throw internview::errors::NotFoundError when user does not exists
     */
    std::optional<std::pair<std::string, std::string>> GetProfilePic(const boost::uuids::uuid& user_id);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::shared_ptr<services::AuthService> auth_service_ptr_;
    internview::services::FileService file_service_;
};

}  // namespace internview::storages

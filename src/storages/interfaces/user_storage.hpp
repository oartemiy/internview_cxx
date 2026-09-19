#pragma once

#include "dto/user_dto.hpp"
#include "models/user.hpp"
#include "userver/server/http/form_data_arg.hpp"

namespace internview::storages::interfaces {

class IUserStorage {
public:
    virtual ~IUserStorage() = default;

    virtual internview::dto::user::ResponseDTO CreateUser(const dto::user::CreateDTO& dto) = 0;

    virtual internview::models::User GetUserById(const boost::uuids::uuid& id) = 0;

    virtual internview::models::User GetUserByLogin(const std::string& login) = 0;

    virtual void UpdatePasswordHash(const boost::uuids::uuid& id, const std::string& new_password_hash) = 0;

    virtual dto::user::ResponseDTO UpdateUser(const dto::user::UpdateDTO& dto) = 0;

    virtual void DeleteUser(const dto::user::DeleteDTO& dto) = 0;

    virtual void UploadProfilePic(const boost::uuids::uuid& id,
                                  const userver::server::http::FormDataArg& file_arg) = 0;

    virtual std::optional<std::pair<std::string, std::string>> GetProfilePic(
        const boost::uuids::uuid& id) = 0;
};

}  // namespace internview::storages::interfaces

#pragma once

#include "models/user.hpp"
#include "userver/server/http/form_data_arg.hpp"


namespace internview::storages::interfaces {

class IUserStorage {
public:
    virtual ~IUserStorage() = default;
    
    virtual internview::models::User GetUserById(const boost::uuids::uuid& id) = 0;

    virtual dto::user::ResponseDTO UpdateUser(const dto::user::UpdateDTO& dto) = 0;

    virtual void DeleteUser(const dto::user::DeleteDTO& dto) = 0;

    virtual void UploadProfilePic(const boost::uuids::uuid& id,
                                  const userver::server::http::FormDataArg& file_arg) = 0;
    
    virtual std::optional<std::pair<std::string, std::string>> GetProfilePic(
        const boost::uuids::uuid& id) = 0;
};

}  // namespace internview::storages::interfaces

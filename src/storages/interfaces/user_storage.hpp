#pragma once

#include "userver/server/http/form_data_arg.hpp"
#include "utils/common_user_storage.hpp"

namespace internview::storages::interfaces {

class IUserStorage {
public:
    virtual ~IUserStorage() = default;
    
    virtual User GetUserById(const boost::uuids::uuid& id) = 0;

    virtual ResponseDTO UpdateUser(const UpdateDTO& dto) = 0;

    virtual void DeleteUser(const DeleteDTO& dto) = 0;

    virtual void UploadProfilePic(const boost::uuids::uuid& id,
                                  const userver::server::http::FormDataArg& file_arg) = 0;

    
    virtual std::optional<std::pair<std::string, std::string>> GetProfilePic(
        const boost::uuids::uuid& id) = 0;
};

}  // namespace internview::storages::interfaces

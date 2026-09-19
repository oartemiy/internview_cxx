#pragma once

#include <memory>

#include "components/storages/user_storage_component.hpp"
#include "storages/interfaces/user_storage.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::services {

class UserService {
public:
    explicit UserService([[maybe_unused]] const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
        : user_storage_(
              component_context.FindComponent<components::UserStorageComponent>().GetStorage()) {
    }

    internview::models::User GetUserById(const boost::uuids::uuid& id) const {
        return user_storage_->GetUserById(id);
    }

    internview::models::User GetUserByLogin(const std::string& login) const {
        return user_storage_->GetUserByLogin(login);
    }

    dto::user::ResponseDTO UpdateUser(const dto::user::UpdateDTO& dto) const {
        return user_storage_->UpdateUser(dto);
    }

    void DeleteUser(const dto::user::DeleteDTO& dto) const {
        return user_storage_->DeleteUser(dto);
    }

    void UploadProfilePic(const boost::uuids::uuid& id,
                          const userver::server::http::FormDataArg& file_arg) const {
        return user_storage_->UploadProfilePic(id, file_arg);
    }

    std::optional<std::pair<std::string, std::string>> GetProfilePic(
        const boost::uuids::uuid& id) const {
        return user_storage_->GetProfilePic(id);
    }

private:
    std::shared_ptr<internview::storages::interfaces::IUserStorage> user_storage_;
};

}  // namespace internview::services

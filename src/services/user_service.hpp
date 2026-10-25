#pragma once

#include <memory>

#include "components/repositories/user_repository_component.hpp"
#include "repositories/interfaces/user_repository.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "utils/file.hpp"

namespace internview::services {

class UserService {
public:
    explicit UserService([[maybe_unused]] const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
        : user_repository_(
              component_context.FindComponent<components::UserRepositoryComponent>().GetRepository()) {
    }

    internview::models::User GetUserById(const boost::uuids::uuid& id) const {
        return user_repository_->GetUserById(id);
    }

    internview::models::User GetUserByLogin(const std::string& login) const {
        return user_repository_->GetUserByLogin(login);
    }

    dto::user::ResponseDTO UpdateUser(const dto::user::UpdateDTO& dto) const {
        return user_repository_->UpdateUser(dto);
    }

    void DeleteUser(const dto::user::DeleteDTO& dto) const {
        return user_repository_->DeleteUser(dto);
    }

    void UploadProfilePic(const boost::uuids::uuid& id,
                          const utils::File& file_arg) const {
        return user_repository_->UploadProfilePic(id, file_arg);
    }

    std::optional<std::pair<std::string, std::string>> GetProfilePic(
        const boost::uuids::uuid& id) const {
        return user_repository_->GetProfilePic(id);
    }

private:
    std::shared_ptr<internview::repositories::interfaces::IUserRepository> user_repository_;
};

}  // namespace internview::services

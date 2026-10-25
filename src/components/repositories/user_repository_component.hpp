#pragma once

#include <memory>
#include <string_view>

#include "repositories/interfaces/user_repository.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"

namespace internview::components {

class UserRepositoryComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "user-repository";

    explicit UserRepositoryComponent(const userver::components::ComponentConfig& config,
                                  const userver::components::ComponentContext& component_context);

    std::shared_ptr<repositories::interfaces::IUserRepository> GetRepository() {
        return repository_;
    }

private:
    std::shared_ptr<repositories::interfaces::IUserRepository> repository_;
};

}  // namespace internview::components

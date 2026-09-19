#pragma once

#include <memory>
#include <string_view>

#include "storages/interfaces/user_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"

namespace internview::components {

class UserStorageComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "user-storage";

    explicit UserStorageComponent(const userver::components::ComponentConfig& config,
                                  const userver::components::ComponentContext& component_context);

    std::shared_ptr<storages::interfaces::IUserStorage> GetStorage() {
        return storage_;
    }

private:
    std::shared_ptr<storages::interfaces::IUserStorage> storage_;
};

}  // namespace internview::components

#pragma once

#include <string_view>

#include "storages/user_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

using userver::components::ComponentConfig;
using userver::components::ComponentContext;

class InternviewComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "internview-component";

    InternviewComponent(const ComponentConfig& config, const ComponentContext& component_context);

    const internview::storages::UserStorage& GetUserStorageRef() const {
        return user_storage_;
    }

private:
    internview::storages::UserStorage user_storage_;
};

}  // namespace internview::components

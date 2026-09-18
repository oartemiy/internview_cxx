#pragma once

#include "storages/interfaces/application_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class ApplicationStorageComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "application-storage-component";

    explicit ApplicationStorageComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::storages::interfaces::IApplicationStorage> GetStorage() {
        return application_storage_;
    }

private:
    std::shared_ptr<internview::storages::interfaces::IApplicationStorage> application_storage_;
};

}  // namespace internview::components

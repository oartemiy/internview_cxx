#pragma once

#include <string_view>

#include "storages/interfaces/refresh_token_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/utils/periodic_task.hpp"
namespace internview::components {

class RefreshTokenStorage final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "refresh-token-storage";

    explicit RefreshTokenStorage(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::storages::interfaces::IRefreshTokenStorage> GetStorage() {
        return refresh_token_storage_;
    }

    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    std::shared_ptr<internview::storages::interfaces::IRefreshTokenStorage> refresh_token_storage_;
    userver::utils::PeriodicTask periodic_task_;
};

}  // namespace internview::components

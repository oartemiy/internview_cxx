#pragma once

#include <string_view>

#include "repositories/interfaces/refresh_token_repository.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/utils/periodic_task.hpp"

namespace internview::components {

class RefreshTokenRepositoryComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "refresh-token-repository";

    explicit RefreshTokenRepositoryComponent(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::repositories::interfaces::IRefreshTokenRepository> GetRepository() {
        return refresh_token_repository_;
    }

    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    std::shared_ptr<internview::repositories::interfaces::IRefreshTokenRepository> refresh_token_repository_;
    userver::utils::PeriodicTask periodic_task_;
};

}  // namespace internview::components

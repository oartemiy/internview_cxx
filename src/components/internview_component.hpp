#pragma once

#include <memory>
#include <string_view>

#include "services/auth_service.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class InternviewComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "internview-component";

    explicit InternviewComponent(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::services::AuthService> GetAuthServicePtr() {
        return auth_service_ptr_;
    }

private:
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
};

}  // namespace internview::components

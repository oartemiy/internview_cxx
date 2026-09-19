#include "auth_service_component.hpp"

#include <memory>

#include "services/auth_service.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

AuthServiceComponent::AuthServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      auth_service_(std::make_shared<services::AuthService>(config, component_context))

{
}

}  // namespace internview::components

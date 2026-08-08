#include "internview_component.hpp"

#include "userver/components/component_base.hpp"

namespace internview::components {

InternviewComponent::InternviewComponent(const ComponentConfig& config,
                                         const ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      user_storage_(component_context) {
}

}  // namespace internview::components

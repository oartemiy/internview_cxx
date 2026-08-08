#include "internview_component.hpp"

#include <sodium.h>

#include <stdexcept>
#include <string>

#include "services/jwt_service.hpp"
#include "sodium/core.h"
#include "userver/components/component_base.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/storages/secdist/component.hpp"

namespace internview::components {

InternviewComponent::InternviewComponent(const ComponentConfig& config,
                                         const ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      user_storage_(component_context)/*,
      jwt_service_(component_context.FindComponent<userver::components::Secdist>()
                                 .Get()
                                 .Get<SecretKey>()
                                 .key)*/ {
    // !NOTE: For password verifing and hashing
    if (sodium_init() != 0) {
        throw std::runtime_error{"Sodium init error"};
    }
}

}  // namespace internview::components

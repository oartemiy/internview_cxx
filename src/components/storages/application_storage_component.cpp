#include "application_storage_component.hpp"

#include "storages/postgres/postgres_application_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

ApplicationStorageComponent::ApplicationStorageComponent(
    const userver::components::ComponentConfig& comfig,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(comfig, component_context),
      application_storage_(
          std::make_shared<internview::storages::postgres::PostgresApplicationStorage>(
              comfig, component_context)) {
}

}  // namespace internview::components

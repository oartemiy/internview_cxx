#include "user_storage_component.hpp"

#include "storages/postgres/postgres_user_storage.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

UserStorageComponent::UserStorageComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context) {
    storage_ = std::make_shared<storages::postgres::PostgresUserStorage>(config, component_context);
}
}  // namespace internview::components

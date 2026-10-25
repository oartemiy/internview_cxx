#include "user_repository_component.hpp"

#include "repositories/postgres/postgres_user_repository.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

UserRepositoryComponent::UserRepositoryComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context) {
    repository_ = std::make_shared<repositories::postgres::PostgresUserRepository>(config, component_context);
}
}  // namespace internview::components

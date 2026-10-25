#include "application_repository_component.hpp"

#include "repositories/postgres/postgres_application_repository.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

ApplicationRepositoryComponent::ApplicationRepositoryComponent(
    const userver::components::ComponentConfig& comfig,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(comfig, component_context),
      application_repository_(
          std::make_shared<internview::repositories::postgres::PostgresApplicationRepository>(
              comfig, component_context)) {
}

}  // namespace internview::components

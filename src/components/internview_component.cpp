#include "internview_component.hpp"

#include <memory>

#include "services/auth_service.hpp"
#include "storages/application_storage.hpp"
#include "storages/vacancy_storage.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

InternviewComponent::InternviewComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      auth_service_ptr_(std::make_shared<services::AuthService>(component_context)),

      vacancy_storage_ptr_(std::make_shared<storages::VacancyStorage>(config, component_context)),
      application_storage_ptr_(
          std::make_shared<storages::ApplicationStorage>(config, component_context)) {
}

}  // namespace internview::components

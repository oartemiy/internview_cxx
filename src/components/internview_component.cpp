#include "internview_component.hpp"

#include <memory>

#include "services/auth_service.hpp"
#include "storages/cv_storage.hpp"
#include "storages/user_storage.hpp"
#include "storages/vacancy_storage.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

InternviewComponent::InternviewComponent(const ComponentConfig& config,
                                         const ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      auth_service_ptr_(std::make_shared<services::AuthService>(component_context)),
      user_storage_ptr_(
          std::make_shared<storages::UserStorage>(auth_service_ptr_, config, component_context)),
      cv_storage_ptr_(
          std::make_shared<storages::CvStorage>(auth_service_ptr_, config, component_context)),
      vacancy_storage_ptr_(std::make_shared<storages::VacancyStorage>(auth_service_ptr_, config,
                                                                      component_context)) {
}

}  // namespace internview::components

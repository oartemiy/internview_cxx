#include "internview_component.hpp"

#include <memory>

#include "services/auth_service.hpp"
#include "storages/application_storage.hpp"
#include "storages/refresh_token_storage.hpp"
#include "storages/vacancy_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/logging/log.hpp"
#include "userver/utils/periodic_task.hpp"

namespace internview::components {

InternviewComponent::InternviewComponent(const userver::components::ComponentConfig& config,
                                         const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      refresh_token_storage_ptr_(std::make_shared<storages::RefreshTokenStorage>(
          config, component_context /* add here lifetime param*/)),
      auth_service_ptr_(
          std::make_shared<services::AuthService>(component_context, refresh_token_storage_ptr_)),
      
      vacancy_storage_ptr_(std::make_shared<storages::VacancyStorage>(config, component_context)),
      application_storage_ptr_(
          std::make_shared<storages::ApplicationStorage>(config, component_context)),
      periodic_task_("clear-expired-tokens",
                     userver::utils::PeriodicTask::Settings(std::chrono::months(1)), [this] {
                         LOG_INFO() << "Stared to cleanup expired tokens";
                         refresh_token_storage_ptr_->ClearExpiredTokens();
                         LOG_INFO() << "Finished to cleanup expired tokens";
                     }) {
}

}  // namespace internview::components

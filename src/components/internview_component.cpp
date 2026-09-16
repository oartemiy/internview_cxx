#include "internview_component.hpp"

#include <memory>

#include "services/auth_service.hpp"
#include "storages/application_storage.hpp"
#include "storages/cv_storage.hpp"
#include "storages/refresh_token_storage.hpp"
#include "storages/user_storage.hpp"
#include "storages/vacancy_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/logging/log.hpp"
#include "userver/utils/periodic_task.hpp"

namespace internview::components {

InternviewComponent::InternviewComponent(const ComponentConfig& config,
                                         const ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      refresh_token_storage_ptr_(std::make_shared<storages::RefreshTokenStorage>(
          config, component_context /* add here lifetime param*/)),
      auth_service_ptr_(
          std::make_shared<services::AuthService>(component_context, refresh_token_storage_ptr_)),
      user_storage_ptr_(
          std::make_shared<storages::UserStorage>(config, component_context)),
      cv_storage_ptr_(
          std::make_shared<storages::CvStorage>(config, component_context)),
      vacancy_storage_ptr_(
          std::make_shared<storages::VacancyStorage>(config, component_context)),
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

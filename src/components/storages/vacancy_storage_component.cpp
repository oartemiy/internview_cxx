#include "vacancy_storage_component.hpp"

#include "storages/postgres/postgres_vacancy_storage.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

VacancyStorageComponent::VacancyStorageComponent(
    const userver::components::ComponentConfig& confing,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(confing, component_context),
      vacancy_storage_(std::make_shared<internview::storages::postgres::PostgresVacancyStorage>(
          confing, component_context)) {
}

}  // namespace internview::components

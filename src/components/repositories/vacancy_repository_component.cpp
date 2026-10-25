#include "vacancy_repository_component.hpp"

#include "repositories/postgres/postgres_vacancy_repository.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

VacancyRepositoryComponent::VacancyRepositoryComponent(
    const userver::components::ComponentConfig& confing,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(confing, component_context),
      vacancy_repository_(std::make_shared<internview::repositories::postgres::PostgresVacancyRepository>(
          confing, component_context)) {
}

}  // namespace internview::components

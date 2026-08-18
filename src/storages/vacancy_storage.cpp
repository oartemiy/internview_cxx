#include "vacancy_storage.hpp"

#include "components/internview_component.hpp"
#include "models/vacancy.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include "vacancy_storage_queries/sql_queries.hpp"

namespace internview::storages {

VacancyStorage::VacancyStorage(std::shared_ptr<internview::services::AuthService> auth_service,
                               [[maybe_unused]] const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      auth_service_ptr_(auth_service) {
}

internview::models::Vacancy VacancyStorage::CreateVacancy(const dto::vacancy::CreateDTO& dto) {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    auto pg_res =
        pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                             vacancy_storage_queries::sql::kCreateVacancy, id, dto.recruiter_id,
                             dto.title, dto.description, dto.requirements, dto.salary_range,
                             dto.location, dto.work_mode, dto.experience_level);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "Title: " + dto.title + " has already taken, rename vacancy"));
    }
    return pg_res.AsSingleRow<models::Vacancy>(userver::storages::postgres::kRowTag);
}

}  // namespace internview::storages

#include "postgres_vacancy_storage.hpp"

#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/utils/boost_uuid7.hpp"
#include "vacancy_storage_queries/sql_queries.hpp"

namespace internview::storages::postgres {

PostgresVacancyStorage::PostgresVacancyStorage(
    [[maybe_unused]] const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()) {
}

internview::models::Vacancy PostgresVacancyStorage::CreateVacancy(
    const dto::vacancy::CreateDTO& dto) {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    try {
        auto pg_res =
            pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                 vacancy_storage_queries::sql::kCreateVacancy, id, dto.recruiter_id,
                                 dto.title, dto.description, dto.requirements, dto.salary_range,
                                 dto.location, dto.work_mode, dto.experience_level);

        return pg_res.AsSingleRow<models::Vacancy>(userver::storages::postgres::kRowTag);
    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "You have already taken this title. Use another one"));
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "This recruiter does not exists. Check authorization"));
    }
}

std::vector<internview::models::Vacancy> PostgresVacancyStorage::GetVacancies(
    const dto::vacancy::GetDTO& dto) {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                       vacancy_storage_queries::sql::kGetVacancies, dto.limit,
                                       dto.offset, dto.query);
    std::vector<models::Vacancy> vec;
    vec.reserve(pg_res.Size());
    for (const auto& row : pg_res) {
        vec.push_back(row.As<models::Vacancy>(userver::v3_1::storages::postgres::kRowTag));
    }
    return vec;
}

internview::models::Vacancy PostgresVacancyStorage::GetVacancyById(const boost::uuids::uuid& id) {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kSlave,
                                       vacancy_storage_queries::sql::kGetVacancyById, id);

    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "Vacancy with id: " + boost::uuids::to_string(id) + " does not exists"));
    }
    auto vacancy = pg_res.AsSingleRow<models::Vacancy>(userver::v3_1::storages::postgres::kRowTag);

    return vacancy;
}

std::vector<internview::models::Vacancy> PostgresVacancyStorage::GetRecruiterVacancies(
    const boost::uuids::uuid& recruiter_id) {
    auto pg_res =
        pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kSlave,
                             vacancy_storage_queries::sql::kGetRecruiterVacancies, recruiter_id);
    std::vector<models::Vacancy> vec;
    vec.reserve(pg_res.Size());
    for (const auto& row : pg_res) {
        vec.push_back(row.As<models::Vacancy>(userver::v3_1::storages::postgres::kRowTag));
    }
    return vec;
}

internview::models::Vacancy PostgresVacancyStorage::UpdateVacancy(
    const dto::vacancy::UpdateDTO& dto) {
    auto model = GetVacancyById(dto.id);
    if (!dto.has_description_in_request_json && !dto.has_experience_level_in_request_json &&
        !dto.has_location_in_request_json && !dto.has_requirements_in_request_json &&
        !dto.has_salary_range_in_request_json && !dto.has_title_in_request_json &&
        !dto.has_work_mode_in_request_json) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "Empty request data body. Nothing to update"));
    }
    int count_changes = 0;
    if (dto.has_title_in_request_json && model.title != dto.title) {
        model.title = dto.title;
        ++count_changes;
    }
    if (dto.has_description_in_request_json && model.description != dto.description) {
        model.description = dto.description;
        ++count_changes;
    }
    if (dto.has_experience_level_in_request_json &&
        model.experience_level != dto.experience_level) {
        model.experience_level = dto.experience_level;
        ++count_changes;
    }
    if (dto.has_location_in_request_json && model.location != dto.location) {
        model.location = dto.location;
        ++count_changes;
    }
    if (dto.has_requirements_in_request_json && model.requirements != dto.requirements) {
        model.requirements = dto.requirements;
        ++count_changes;
    }
    if (dto.has_work_mode_in_request_json && model.work_mode != dto.work_mode) {
        model.work_mode = dto.work_mode;
        ++count_changes;
    }
    if (dto.has_salary_range_in_request_json && model.salary_range != dto.salary_range) {
        model.salary_range = dto.salary_range;
        ++count_changes;
    }
    if (count_changes == 0) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Nothing to update"));
    }
    try {
        auto pg_res = pg_cluster_->Execute(
            userver::v3_1::storages::postgres::ClusterHostType::kMaster,
            vacancy_storage_queries::sql::kUpdateVacancy, dto.id, dto.recruiter_id, model.title,
            model.description, model.requirements, model.salary_range, model.location,
            model.work_mode, model.experience_level);
    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "You have already taken this title. Use another one"));
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "This recruiter does not exists. Check authorization"));
    }
    return model;
}

void PostgresVacancyStorage::DeleteVacancy(const boost::uuids::uuid& id,
                                           const boost::uuids::uuid& recruiter_id) {
    auto pg_res =
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                             vacancy_storage_queries::sql::kDeleteVacancy, id, recruiter_id);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Vacancy with id " + boost::uuids::to_string(id) + " does not exists"));
    }
}

internview::models::Vacancy PostgresVacancyStorage::ToggleVacancy(
    const boost::uuids::uuid& id, const boost::uuids::uuid& recruiter_id) {
    try {
        auto pg_res =
            pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                 vacancy_storage_queries::sql::kToggleVacancy, id, recruiter_id);
        if (pg_res.IsEmpty()) {
            throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
                "message", "Vacancy with id " + boost::uuids::to_string(id) + " does not exists"));
        }
        return pg_res.AsSingleRow<models::Vacancy>(userver::v3_1::storages::postgres::kRowTag);
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "This recruiter does not exists. Check authorization"));
    }
}

boost::uuids::uuid PostgresVacancyStorage::GetRecruiterIdByApplicationId(
    const boost::uuids::uuid& application_id) {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kSlave,
                                       vacancy_storage_queries::sql::kGetRecruiterIdByApplicationId,
                                       application_id);
    return pg_res.AsSingleRow<boost::uuids::uuid>();
}

}  // namespace internview::storages::postgres

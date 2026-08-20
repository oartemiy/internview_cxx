#include "application_storage.hpp"

#include <userver/storages/postgres/exceptions.hpp>
#include <vector>

#include "application_storage_queries/sql_queries.hpp"
#include "models/application.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/exceptions.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/utils/boost_uuid7.hpp"

namespace internview::storages {

ApplicationStorage::ApplicationStorage(
    [[maybe_unused]] const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()) {
}

models::Application ApplicationStorage::GetApplicationById(const boost::uuids::uuid& id) {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                       application_storage_queries::sql::kGetApplicationById, id);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "This application does not exists"));
    }
    return pg_res.AsSingleRow<models::Application>(userver::v3_1::storages::postgres::kRowTag);
}

models::Application ApplicationStorage::CreateApplication(const dto::application::CreateDTO& dto) {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    try {
        auto pg_res =
            pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                 application_storage_queries::sql::kCreateApplication, id,
                                 dto.vacancy_id, dto.intern_id, dto.cv_id, dto.cover_letter);
        auto model =
            pg_res.AsSingleRow<models::Application>(userver::v3_1::storages::postgres::kRowTag);
        return model;

    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "You have already applied for this vacancy"));
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "This vacancy or cv does not exists"));
    }
}

std::vector<models::Application> ApplicationStorage::GetInternsApplications(
    const boost::uuids::uuid& intern_id) {
    auto pg_res =
        pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                             application_storage_queries::sql::kGetInternsApplications, intern_id);
    std::vector<models::Application> vec;
    vec.reserve(pg_res.Size());
    for (const auto& row : pg_res) {
        vec.push_back(row.As<models::Application>(userver::v3_1::storages::postgres::kRowTag));
    }
    return vec;
}

std::vector<models::Application> ApplicationStorage::GetRecruiterApplications(
    const boost::uuids::uuid& recruiter_id) {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                       application_storage_queries::sql::kGetRecruiterApplications,
                                       recruiter_id);
    std::vector<models::Application> vec;
    vec.reserve(pg_res.Size());
    for (const auto& row : pg_res) {
        vec.push_back(row.As<models::Application>(userver::v3_1::storages::postgres::kRowTag));
    }
    return vec;
}

std::vector<models::Application> ApplicationStorage::GetVacancyApplications(
    const boost::uuids::uuid& vacancy_id) {
    auto pg_res =
        pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                             application_storage_queries::sql::kGetVacancyApplications, vacancy_id);
    std::vector<models::Application> res_vec;
    res_vec.reserve(pg_res.Size());
    for (const auto& row : pg_res) {
        res_vec.push_back(row.As<models::Application>(userver::v3_1::storages::postgres::kRowTag));
    }
    return res_vec;
}

models::Application ApplicationStorage::UpdateApplication(const dto::application::UpdateDTO& dto) {
    auto model = GetApplicationById(dto.id);
    if (dto.has_cover_letter_in_request_json) {
        model.cover_letter = dto.cover_letter;
    }
    if (dto.has_cv_id_in_request_json) {
        model.cv_id = dto.cv_id;
    }
    if (dto.has_status_in_request_json) {
        model.status = dto.status;
    }
    try {
        auto pg_res =
            pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                 application_storage_queries::sql::kUpdateApplication, model.id,
                                 model.cv_id, model.status, model.cover_letter);
        return pg_res.AsSingleRow<models::Application>(userver::v3_1::storages::postgres::kRowTag);
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "invalid cv_pdf"));
    } catch (userver::storages::postgres::CheckViolation& e) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "unsupported status"));
    }
}

void ApplicationStorage::DeleteApplication(const boost::uuids::uuid& id,
                                           const boost::uuids::uuid& intern_id) {
    auto pg_res =
        pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                             application_storage_queries::sql::kDeleteApplication, id, intern_id);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "This application does not exist"));
    }
}

bool ApplicationStorage::CheckInternApplied(const boost::uuids::uuid& intern_id,
                                            const boost::uuids::uuid& recruiter_id) {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kMaster,
                                       application_storage_queries::sql::kCheckInternApplied,
                                       intern_id, recruiter_id);
    if (pg_res.IsEmpty()) {
        return false;
    }
    return true;
}

boost::uuids::uuid ApplicationStorage::GetInternIdByCv(const boost::uuids::uuid& cv_id,
                                                       const boost::uuids::uuid& recruiter_id) {
    auto pg_res = pg_cluster_->Execute(userver::v3_1::storages::postgres::ClusterHostType::kSlave,
                                       application_storage_queries::sql::kGetInternIdByCv, cv_id,
                                       recruiter_id);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "This cv does not belongs to your vacancies applications"));
    }
    return pg_res.AsSingleRow<boost::uuids::uuid>();
}

}  // namespace internview::storages

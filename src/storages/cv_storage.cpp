#include "cv_storage.hpp"

#include <chrono>
#include <vector>

#include "cv_storage_queries/sql_queries.hpp"
#include "dto/cv_dto.hpp"
#include "models/cv.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/utils/boost_uuid7.hpp"

namespace internview::storages {

CvStorage::CvStorage(std::shared_ptr<services::AuthService> auth_service_ptr,
                     const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      auth_service_ptr_(auth_service_ptr),
      file_service_(config, component_context) {
}

dto::cv::ResponseDTO CvStorage::CreateCv(const dto::cv::CreateDTO& dto) const {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       cv_storage_queries::sql::kCreateCv, id, dto.user_id,
                                       dto.title, dto.description, dto.cv_pdf);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Title: " + dto.title + " has already taken, rename cv"));
    }
    auto created_at = pg_res[0][0].As<std::chrono::system_clock::time_point>();
    dto::cv::ResponseDTO resp_dto = {id,         dto.user_id, dto.title, dto.description,
                                     dto.cv_pdf, created_at,  created_at};
    return resp_dto;
}

std::vector<internview::models::CV> CvStorage::GetUserCvs(const boost::uuids::uuid& user_id) const {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       cv_storage_queries::sql::kGetCvs, user_id);
    std::vector<internview::models::CV> res_vec;
    res_vec.reserve(pg_res.Size());
    // TODO: optimize with emplace_back method
    for (const auto& row : pg_res) {
        res_vec.push_back(row.As<internview::models::CV>(userver::storages::postgres::kRowTag));
    }
    return res_vec;
}

internview::models::CV CvStorage::GetCvById(const boost::uuids::uuid& id,
                                            const boost::uuids::uuid& user_id) const {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       cv_storage_queries::sql::kGetCvById, id, user_id);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message",
            "Cv id: " + boost::uuids::to_string(id) + " is unavailable for current user"));
    }
    auto models = pg_res.AsSingleRow<models::CV>(userver::storages::postgres::kRowTag);
    return models;
}

internview::dto::cv::ResponseDTO CvStorage::UpdateCv(
    const internview::dto::cv::UpdateDTO& dto) const {
    auto cv_model = GetCvById(dto.id, dto.user_id);

    auto new_title = dto.title ? *dto.title : cv_model.title;
    auto new_description = dto.description ? *dto.description : cv_model.description;
    auto new_cv_pdf = dto.cv_pdf ? *dto.cv_pdf : cv_model.cv_pdf;

    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       cv_storage_queries::sql::kUpdateCv, dto.id, dto.user_id,
                                       new_title, new_description, new_cv_pdf);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Cv may not exists or title has already taken in user's cvs"));
    }
    auto updated_at = pg_res[0][0].As<std::chrono::system_clock::time_point>();
    return {dto.id,     dto.user_id,         new_title, new_description,
            new_cv_pdf, cv_model.created_at, updated_at};
}

}  // namespace internview::storages

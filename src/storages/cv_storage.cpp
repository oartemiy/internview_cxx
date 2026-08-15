#include "cv_storage.hpp"

#include <chrono>
#include <vector>

#include "cv_storage_queries/sql_queries.hpp"
#include "dto/cv_dto.hpp"
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

}  // namespace internview::storages

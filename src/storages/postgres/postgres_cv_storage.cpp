#include "postgres_cv_storage.hpp"

#include "components/pdf_storage_component.hpp"
#include "cv_storage_queries/sql_queries.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/utils/boost_uuid7.hpp"

namespace internview::storages::postgres {

PostgresCvStorage::PostgresCvStorage(
    [[maybe_unused]] const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : pg_cluster_(component_context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster()),
      pdf_storage_(component_context.FindComponent<internview::components::PdfStorageComponent>()
                       .GetStorage()) {
}

dto::cv::ResponseDTO PostgresCvStorage::CreateCv(const dto::cv::CreateDTO& dto) {
    auto id = userver::utils::generators::GenerateBoostUuidV7();
    try {
        auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                           cv_storage_queries::sql::kCreateCv, id, dto.user_id,
                                           dto.title, dto.description, dto.cv_pdf);
        auto created_at = pg_res[0][0].As<std::chrono::system_clock::time_point>();
        dto::cv::ResponseDTO resp_dto = {id,         dto.user_id, dto.title, dto.description,
                                         dto.cv_pdf, created_at,  created_at};
        return resp_dto;
    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Title: " + dto.title + " has already taken, rename cv"));
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "This intern does not exists. Check authrization"));
    }
}

std::vector<models::CV> PostgresCvStorage::GetUserCvs(const boost::uuids::uuid& user_id) {
    try {
        auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                           cv_storage_queries::sql::kGetCvs, user_id);
        std::vector<models::CV> res_vec;
        res_vec.reserve(pg_res.Size());
        for (const auto& row : pg_res) {
            res_vec.push_back(row.As<models::CV>(userver::storages::postgres::kRowTag));
        }
        return res_vec;
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ResourceNotFound(
            userver::formats::json::MakeObject("message", "This user does not exists"));
    }
}

models::CV PostgresCvStorage::GetCvById(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) {
    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                       cv_storage_queries::sql::kGetCvById, id, user_id);
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message",
            "Cv id: " + boost::uuids::to_string(id) + " is unavailable for current user"));
    }
    auto models = pg_res.AsSingleRow<models::CV>(userver::storages::postgres::kRowTag);
    return models;
}

dto::cv::ResponseDTO PostgresCvStorage::UpdateCv(const internview::dto::cv::UpdateDTO& dto) {
    if (!dto.has_cv_pdf_in_request && !dto.has_description_in_request &&
        !dto.has_title_in_request) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "Empty request data body. Nothing to update"));
    }
    auto model = GetCvById(dto.id, dto.user_id);
    auto old_cv_pdf = model.cv_pdf;
    int count_changes = 0;

    if (dto.has_title_in_request && model.title != dto.title) {
        model.title = dto.title;
        ++count_changes;
    }
    if (dto.has_description_in_request && model.description != dto.description) {
        model.description = dto.description;
        ++count_changes;
    }
    if (dto.has_cv_pdf_in_request && model.cv_pdf != dto.cv_pdf) {
        model.cv_pdf = dto.cv_pdf;
        ++count_changes;
    }
    if (count_changes == 0) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Nothing to update"));
    }
    if (model.title.length() <= 1) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Title must be at least 2 chars"));
    }

    try {
        auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                           cv_storage_queries::sql::kUpdateCv, dto.id, model.title,
                                           model.description, model.cv_pdf);
        if (old_cv_pdf != std::nullopt && model.cv_pdf == std::nullopt) {
            pdf_storage_->Delete(*old_cv_pdf);
        }
        auto updated_at = pg_res[0][0].As<std::chrono::system_clock::time_point>();
        return {dto.id,       dto.user_id,      model.title, model.description,
                model.cv_pdf, model.created_at, updated_at};
    } catch (userver::storages::postgres::UniqueViolation& e) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "Cv may not exists or title has already taken in user's cvs"));
    } catch (userver::storages::postgres::ForeignKeyViolation& e) {
        throw userver::server::handlers::ConflictError(userver::formats::json::MakeObject(
            "message", "This intern does not exists. Check authrization"));
    }
}

void PostgresCvStorage::DeleteCv(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) {
    auto cv_model = GetCvById(id, user_id);

    auto pg_res = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                       cv_storage_queries::sql::kDeleteCv, id, user_id);
    if (cv_model.cv_pdf != std::nullopt) {
        pdf_storage_->Delete(*cv_model.cv_pdf);
    }
    if (pg_res.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Invalid id or user_id"));
    }
}

void PostgresCvStorage::UploadCvPdf(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id,
                                    const userver::server::http::FormDataArg& file_arg) {

    auto cv_model = GetCvById(id, user_id);

    if (cv_model.cv_pdf != std::nullopt) {
        pdf_storage_->Delete(*cv_model.cv_pdf);
    }
    auto key = pdf_storage_->Save(file_arg);

    UpdateCv({false, false, true, id, user_id, "", std::nullopt, key});
}

std::optional<std::pair<std::string, std::string>> PostgresCvStorage::GetCvPdf(
    const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) {
    auto cv_model = GetCvById(id, user_id);
    if (!cv_model.cv_pdf) {
        return std::nullopt;
    }
    auto pic = cv_model.cv_pdf;
    try {
        auto file = pdf_storage_->Load(*pic);
        std::pair<std::string, std::string> res{*pic, file};
        return res;
    } catch (std::runtime_error& e) {
        return std::nullopt;
    }
}

}  // namespace internview::storages::postgres

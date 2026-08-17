#pragma once

#include <userver/engine/task/task_processor_fwd.hpp>
#include <userver/server/http/form_data_arg.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>

#include "dto/cv_dto.hpp"
#include "models/cv.hpp"
#include "services/auth_service.hpp"
#include "services/file_service.hpp"

// TODO: add @throw section
namespace internview::storages {

// NOTE: internview::models::CV and internview::dto::cv::ResponseDTO have the same feilds
// BUT: dto does not support postgres deserialization (architecture decision)
// AND: models support json serialization
class CvStorage {
public:
    explicit CvStorage(std::shared_ptr<services::AuthService> auth_service_ptr,
                       const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& component_context);

    /**
     * @brief Create a Cv object
     *
     * @param dto
     * @return dto::cv::ResponseDTO
     * @throw userver::server::handlers::ConflictError
     */
    dto::cv::ResponseDTO CreateCv(const dto::cv::CreateDTO& dto) const;

    /**
     * @brief Get the User Cvs objects
     *
     * @param user_id
     * @return std::vector<internview::models::CV>
     */
    std::vector<internview::models::CV> GetUserCvs(const boost::uuids::uuid& user_id) const;

    /**
     * @brief Get the Cv By Id object
     *
     * @param id
     * @param user_id
     * @return internview::models::CV
     * NOTE: To delele optional field use DELETE in a json_request
     */
    internview::models::CV GetCvById(const boost::uuids::uuid& id,
                                     const boost::uuids::uuid& user_id) const;

    /**
     * @brief Update CV object
     *
     * @param dto
     * @return internview::dto::cv::ResponseDTO
     */
    internview::dto::cv::ResponseDTO UpdateCv(const internview::dto::cv::UpdateDTO& dto) const;

    /**
     * @brief Delete CV object
     *
     * @param id
     * @param user_id
     */
    void DeleteCv(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) const;

    /**
     * @brief Upload cv_pdf
     * 
     * @param id 
     * @param user_id 
     * @param file_arg 
     */
    void UploadCvPdf(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id,
                     const userver::server::http::FormDataArg& file_arg);

    /**
     * @brief Get the Cv Pdf object
     * 
     * @param id 
     * @param user_id 
     * @return std::optional<std::pair<std::string, std::string>> 
     */
    std::optional<std::pair<std::string, std::string>> GetCvPdf(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::shared_ptr<services::AuthService> auth_service_ptr_;
    internview::services::FileService file_service_;
};

}  // namespace internview::storages

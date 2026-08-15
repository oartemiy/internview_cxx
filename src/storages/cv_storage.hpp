#pragma once

#include <userver/engine/task/task_processor_fwd.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "dto/cv_dto.hpp"
#include "services/auth_service.hpp"
#include "services/file_service.hpp"

namespace internview::storages {

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
     * @throw std::runtime_error
     */
    dto::cv::ResponseDTO CreateCv(const dto::cv::CreateDTO& dto) const;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::shared_ptr<services::AuthService> auth_service_ptr_;
    internview::services::FileService file_service_;
};

}  // namespace internview::storages

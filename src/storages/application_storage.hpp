#pragma once

#include <userver/storages/postgres/cluster.hpp>
#include <vector>

#include "dto/application_dto.hpp"
#include "models/application.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

namespace internview::storages {
// NOTE: Application dto (as vacancy does not have ResponseDTO), only model
class ApplicationStorage {
public:
    ApplicationStorage(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& component_context);

    /**
     * @brief Create a Application object
     * 
     * @param dto 
     * @return models::Application 
     */
    models::Application CreateApplication(const dto::application::CreateDTO& dto);

    std::vector<models::Application> GetInternsApplications(const boost::uuids::uuid& intern_id);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace internview::storages

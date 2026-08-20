#pragma once

#include <optional>
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

    /**
     * @brief Get the Interns Applications object
     *
     * @param intern_id
     * @return std::vector<models::Application>
     */
    std::vector<models::Application> GetInternsApplications(const boost::uuids::uuid& intern_id);

    /**
     * @brief Get the Recruiter Applications object
     *
     * @param recruiter_id
     * @return std::vector<models::Application>
     */
    std::vector<models::Application> GetRecruiterApplications(
        const boost::uuids::uuid& recruiter_id);

    /**
     * @brief Get the Vacancy Applications object
     *
     * @param vacancy_id
     * @return std::vector<models::Application>
     */
    std::vector<models::Application> GetVacancyApplications(const boost::uuids::uuid& vacancy_id);

    /**
     * @brief Get the Application By Id object
     *
     * @param id
     * @return models::Application
     */
    models::Application GetApplicationById(const boost::uuids::uuid& id);

    /**
     * @brief Update the application object
     *
     * @param dto
     * @return models::Application
     */
    models::Application UpdateApplication(const dto::application::UpdateDTO& dto);

    /**
     * @brief Delete Application object
     * 
     * @param intern_id 
     */
    void DeleteApplication(const boost::uuids::uuid& id, const boost::uuids::uuid& intern_id);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace internview::storages

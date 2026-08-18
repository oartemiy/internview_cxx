#pragma once

#include <cstddef>
#include <vector>

#include "dto/vacancy_dto.hpp"
#include "models/vacancy.hpp"
#include "services/auth_service.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

// NOTE ResponseDTO for Vacancy does not provided
// TODO: Replace ResponseDTO for most models
namespace internview::storages {

class VacancyStorage {
public:
    VacancyStorage(std::shared_ptr<internview::services::AuthService> auth_service,
                   const userver::components::ComponentConfig& config,
                   const userver::components::ComponentContext& component_context);

    /**
     * @brief Create a Vacancy object
     *
     * @param dto
     * @return internview::models::Vacancy
     */
    internview::models::Vacancy CreateVacancy(const dto::vacancy::CreateDTO& dto);

    /**
     * @brief Get the Vacancies object
     *
     * @param limit
     * @param offset
     * @return std::vector<internview::models::Vacancy>
     */
    std::vector<internview::models::Vacancy> GetVacancies(int limit, int offset);

    /**
     * @brief Get the Vacancy By Id object
     *
     * @param id
     * @return internview::models::Vacancy
     */
    internview::models::Vacancy GetVacancyById(const boost::uuids::uuid& id);

    /**
     * @brief Get the Recruiter Vacancies objects
     *
     * @param recruiter_id
     * @return std::vector<internview::models::Vacancy>
     */
    std::vector<internview::models::Vacancy> GetRecruiterVacancies(
        const boost::uuids::uuid& recruiter_id);

    /**
     * @brief Update the Vacancy
     *
     * @param dto
     * @return internview::models::Vacancy
     */
    internview::models::Vacancy UpdateVacancy(const dto::vacancy::UpdateDTO& dto);


    /**
     * @brief Delete the Vacancy
     * 
     * @param id 
     * @param recruiter_id 
     */
    void DeleteVacancy(const boost::uuids::uuid& id, const boost::uuids::uuid& recruiter_id);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
};

}  // namespace internview::storages

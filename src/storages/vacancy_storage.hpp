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
     * @throw userver::server::handlers::ClientError
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
     * @throw userver::server::handlers::ClientError
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
     * @throw userver::server::handlers::ClientError
     * @return internview::models::Vacancy
     */
    internview::models::Vacancy UpdateVacancy(const dto::vacancy::UpdateDTO& dto);

    /**
     * @brief Delete the Vacancy
     *
     * @param id
     * @param recruiter_id
     * @throw userver::server::handlers::ConflictError
     */
    void DeleteVacancy(const boost::uuids::uuid& id, const boost::uuids::uuid& recruiter_id);

    /**
     * @brief Sets is_active flag to opposite position
     *
     * @param id
     * @param recruiter_id
     * @return internview::models::Vacancy
     * @throw userver::server::handlers::ClientError
     */
    internview::models::Vacancy ToggleVacancy(const boost::uuids::uuid& id,
                                              const boost::uuids::uuid& recruiter_id);

    /**
     * @brief Get the Recruiter Id By Application Id object
     *
     * @param application_id
     * @return boost::uuids::uuid
     */
    boost::uuids::uuid GetRecruiterIdByApplicationId(const boost::uuids::uuid& application_id);

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
};

}  // namespace internview::storages

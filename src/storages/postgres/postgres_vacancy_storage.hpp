#pragma once

#include "storages/interfaces/vacancy_storage.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

namespace internview::storages::postgres {

class PostgresVacancyStorage final : public internview::storages::interfaces::IVacancyStorage {
public:
    explicit PostgresVacancyStorage(const userver::components::ComponentConfig& config,
                                    const userver::components::ComponentContext& component_context);

    ~PostgresVacancyStorage() override = default;
    PostgresVacancyStorage(const PostgresVacancyStorage&) = delete;
    PostgresVacancyStorage& operator=(const PostgresVacancyStorage&) = delete;
    PostgresVacancyStorage(PostgresVacancyStorage&&) = delete;
    PostgresVacancyStorage& operator=(PostgresVacancyStorage&&) = delete;

    /**
     * @brief Create a Vacancy object
     *
     * @param dto
     * @return internview::models::Vacancy
     * @throw userver::server::handlers::ClientError
     */
    internview::models::Vacancy CreateVacancy(const dto::vacancy::CreateDTO& dto) override;

    /**
     * @brief Get the Vacancies object
     *
     * @param limit
     * @param offset
     * @return std::vector<internview::models::Vacancy>
     */
    std::vector<internview::models::Vacancy> GetVacancies(const dto::vacancy::GetDTO& dto) override;

    /**
     * @brief Get the Vacancy By Id object
     *
     * @param id
     * @return internview::models::Vacancy
     * @throw userver::server::handlers::ClientError
     */
    internview::models::Vacancy GetVacancyById(const boost::uuids::uuid& id) override;

    /**
     * @brief Get the Recruiter Vacancies objects
     *
     * @param recruiter_id
     * @return std::vector<internview::models::Vacancy>
     */
    std::vector<internview::models::Vacancy> GetRecruiterVacancies(
        const boost::uuids::uuid& recruiter_id) override;

    /**
     * @brief Update the Vacancy
     *
     * @param dto
     * @throw userver::server::handlers::ClientError
     * @return internview::models::Vacancy
     */
    internview::models::Vacancy UpdateVacancy(const dto::vacancy::UpdateDTO& dto) override;

    /**
     * @brief Delete the Vacancy
     *
     * @param id
     * @param recruiter_id
     * @throw userver::server::handlers::ConflictError
     */
    void DeleteVacancy(const boost::uuids::uuid& id,
                       const boost::uuids::uuid& recruiter_id) override;

    /**
     * @brief Sets is_active flag to opposite position
     *
     * @param id
     * @param recruiter_id
     * @return internview::models::Vacancy
     * @throw userver::server::handlers::ClientError
     */
    internview::models::Vacancy ToggleVacancy(const boost::uuids::uuid& id,
                                              const boost::uuids::uuid& recruiter_id) override;

    /**
     * @brief Get the Recruiter Id By Application Id object
     *
     * @param application_id
     * @return boost::uuids::uuid
     */
    boost::uuids::uuid GetRecruiterIdByApplicationId(
        const boost::uuids::uuid& application_id) override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};
}  // namespace internview::storages::postgres

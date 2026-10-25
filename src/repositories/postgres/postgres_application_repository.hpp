#pragma once

#include <userver/storages/postgres/postgres_fwd.hpp>

#include "repositories/interfaces/application_repository.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::repositories::postgres {

class PostgresApplicationRepository final
    : public internview::repositories::interfaces::IApplicationRepository {
public:
    explicit PostgresApplicationRepository(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context);

    ~PostgresApplicationRepository() override = default;
    PostgresApplicationRepository(const PostgresApplicationRepository&) = delete;
    PostgresApplicationRepository& operator=(PostgresApplicationRepository) = delete;
    PostgresApplicationRepository(PostgresApplicationRepository&&) = delete;
    PostgresApplicationRepository& operator=(PostgresApplicationRepository&&) = delete;

    /**
     * @brief Create a Application object
     *
     * @param dto
     * @return models::Application
     * @throw userver::server::handlers::ClientError
     */
    models::Application CreateApplication(const dto::application::CreateDTO& dto) override;

    /**
     * @brief Get the Interns Applications object
     *
     * @param intern_id
     * @return std::vector<models::Application>
     */
    std::vector<models::Application> GetInternsApplications(
        const boost::uuids::uuid& intern_id) override;

    /**
     * @brief Get the Recruiter Applications object
     *
     * @param recruiter_id
     * @return std::vector<models::Application>
     */
    std::vector<models::Application> GetRecruiterApplications(
        const boost::uuids::uuid& recruiter_id) override;

    /**
     * @brief Get the Vacancy Applications object
     *
     * @param vacancy_id
     * @return std::vector<models::Application>
     */
    std::vector<models::Application> GetVacancyApplications(
        const boost::uuids::uuid& vacancy_id) override;

    /**
     * @brief Get the Application By Id object
     *
     * @param id
     * @return models::Application
     */
    models::Application GetApplicationById(const boost::uuids::uuid& id) override;

    /**
     * @brief Update the application object
     *
     * @param dto
     * @return models::Application
     * @throw userver::server::handlers::ClientError
     */
    models::Application UpdateApplication(const dto::application::UpdateDTO& dto) override;

    /**
     * @brief Delete Application object
     *
     * @param intern_id
     * @throw userver::server::handlers::ClientError
     */
    void DeleteApplication(const boost::uuids::uuid& id,
                           const boost::uuids::uuid& intern_id) override;

    /**
     * @brief Checks interns is applied
     *
     * @param intern_id
     * @return true
     * @return false
     */
    bool CheckInternApplied(const boost::uuids::uuid& intern_id,
                            const boost::uuids::uuid& recruiter_id) override;

    /**
     * @brief Get the Intern Id By Cv object
     *
     * @param cv_id
     * @param recruiter_id
     * @return boost::uuids::uuid
     * @throw userver::server::handlers::ClientError
     */
    boost::uuids::uuid GetInternIdByCv(const boost::uuids::uuid& cv_id,
                                       const boost::uuids::uuid& recruiter_id) override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};
}  // namespace internview::repositories::postgres

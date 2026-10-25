#pragma once

#include "repositories/interfaces/cv_repository.hpp"
#include "storages/interfaces/file_storage.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"
#include "utils/file.hpp"

namespace internview::repositories::postgres {

class PostgresCvRepository final : public internview::repositories::interfaces::ICvRepository {
public:
    explicit PostgresCvRepository(const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& component_context);

    ~PostgresCvRepository() override = default;
    PostgresCvRepository(const PostgresCvRepository&) = delete;
    PostgresCvRepository& operator=(const PostgresCvRepository&) = delete;
    PostgresCvRepository(PostgresCvRepository&&) = delete;
    PostgresCvRepository& operator=(PostgresCvRepository&&) = delete;

    /**
     * @brief Create a Cv object
     *
     * @param dto
     * @return dto::cv::ResponseDTO
     * @throw userver::server::handlers::ConflictError
     */
    dto::cv::ResponseDTO CreateCv(const dto::cv::CreateDTO& dto) override;

    /**
     * @brief Get the User Cvs objects
     *
     * @param user_id
     * @return std::vector<internview::models::CV>
     * @throw userver::server::handlers::ResourceNotFound
     */
    std::vector<models::CV> GetUserCvs(const boost::uuids::uuid& user_id) override;

    /**
     * @brief Get the Cv By Id object
     *
     * @param id
     * @param user_id
     * @return internview::models::CV
     */
    models::CV GetCvById(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) override;

    /**
     * @brief Update CV object
     *
     * @param dto
     * @return internview::dto::cv::ResponseDTO
     * @throw userver::server::handlers::ClientError
     */
    dto::cv::ResponseDTO UpdateCv(const internview::dto::cv::UpdateDTO& dto) override;

    /**
     * @brief Delete CV object
     *
     * @param id
     * @param user_id
     * @throw userver::server::handlers::ClientError
     */
    void DeleteCv(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) override;

    /**
     * @brief Upload cv_pdf
     *
     * @param id
     * @param user_id
     * @param file_arg
     * @throw userver::server::handlers::ClientError
     */
    void UploadCvPdf(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id,
                     const utils::File& file) override;

    /**
     * @brief Get the Cv Pdf object
     *
     * @param id
     * @param user_id
     * @return std::optional<std::pair<std::string, std::string>>
     */
    std::optional<std::pair<std::string, std::string>> GetCvPdf(
        const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::shared_ptr<internview::storages::interfaces::IFileStorage> pdf_storage_;
};
}  // namespace internview::repositories::postgres

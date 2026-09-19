#pragma once

#include <userver/storages/postgres/cluster.hpp>

#include "dto/user_dto.hpp"
#include "models/user.hpp"
#include "storages/interfaces/file_storage.hpp"
#include "storages/interfaces/user_storage.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::storages::postgres {

class PostgresUserStorage final : public internview::storages::interfaces::IUserStorage {
public:
    explicit PostgresUserStorage(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context);

    ~PostgresUserStorage() override = default;
    PostgresUserStorage(const PostgresUserStorage&) = delete;
    PostgresUserStorage& operator=(const PostgresUserStorage&) = delete;
    PostgresUserStorage(PostgresUserStorage&&) = delete;
    PostgresUserStorage& operator=(PostgresUserStorage&&) = delete;

    /**
     * @brief Creates a User object
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throws userver::server::handlers::ConflictError
               std::runtime_error
     */
    internview::dto::user::ResponseDTO CreateUser(const dto::user::CreateDTO& dto) override;

    /**
     * @brief Get the User By Id object
     *
     * @param id
     * @return User
     * @throw userver::server::handlers::ResourceNotFound
     */
    internview::models::User GetUserById(const boost::uuids::uuid& id) override;

    /**
     * @brief Get the User object By login
     *
     * @param login
     * @return internview::models::User
     */
    internview::models::User GetUserByLogin(const std::string& login) override;

    /**
     * @brief Changes user password
     *
     * @param dto
     * @return boost::uuids::uuid
     */
    void UpdatePasswordHash(const boost::uuids::uuid& id,
                            const std::string& new_password_hash) override;

    /**
     * @brief Update the User object
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throws userver::server::handlers::ConflictError
               userver::server::handlers::ResourceNotFound
     */
    internview::dto::user::ResponseDTO UpdateUser(
        const internview::dto::user::UpdateDTO& dto) override;

    /**
     * @brief Delete the User object
     *
     * @param dto
     * @throws userver::server::handlers::ClientError
               userver::server::handlers::ResourceNotFound
     */
    void DeleteUser(const dto::user::DeleteDTO& dto) override;

    /**
     * @brief Uploads and sets new user's profile picture
     *
     * @param id
     * @param description
     * @throws userver::server::handlers::ClientError
               std::runtime_error
     */
    void UploadProfilePic(const boost::uuids::uuid& id,
                          const userver::server::http::FormDataArg& file_arg) override;

    /**
     * @brief Get the Profile Pic object
     *
     * @param id
     * @return std::optional<std::pair<std::string, std::string>> 'key, which ends with file
     extension; 'file_data
     * @throws userver::server::handlers::ResourceNotFound
               std::runtime_error
     */
    std::optional<std::pair<std::string, std::string>> GetProfilePic(
        const boost::uuids::uuid& id) override;

private:
    userver::engine::TaskProcessor& crypto_tp_;
    userver::storages::postgres::ClusterPtr pg_cluster_;
    std::shared_ptr<interfaces::IFileStorage> img_storage_;
};
}  // namespace internview::storages::postgres

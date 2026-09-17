#pragma once

#include <userver/storages/postgres/cluster.hpp>

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
     * @brief Get the User By Id object
     *
     * @param id
     * @return User
     * @throw userver::server::handlers::ResourceNotFound
     */
    User GetUserById(const boost::uuids::uuid& id) override;

    /**
     * @brief Update the User object
     *
     * @param dto
     * @return dto::user::ResponseDTO
     * @throws userver::server::handlers::ConflictError
               userver::server::handlers::ResourceNotFound
     */
    ResponseDTO UpdateUser(const UpdateDTO& dto) override;

    /**
     * @brief Delete the User object
     *
     * @param dto
     * @throws userver::server::handlers::ClientError
               userver::server::handlers::ResourceNotFound
     */
    void DeleteUser(const DeleteDTO& dto) override;

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

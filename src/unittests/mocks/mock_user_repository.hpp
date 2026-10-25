#pragma once

#include <gmock/gmock.h>

#include "repositories/interfaces/user_repository.hpp"

namespace internview::mocks {

class MockUserRepository : public repositories::interfaces::IUserRepository {
public:
    MOCK_METHOD(dto::user::ResponseDTO, CreateUser, (const dto::user::CreateDTO& dto), (override));
    MOCK_METHOD(models::User, GetUserById, (const boost::uuids::uuid& id), (override));
    MOCK_METHOD(models::User, GetUserByLogin, (const std::string& login), (override));
    MOCK_METHOD(void, UpdatePasswordHash,
                (const boost::uuids::uuid& id, const std::string& new_password_hash), (override));
    MOCK_METHOD(dto::user::ResponseDTO, UpdateUser, (const dto::user::UpdateDTO& dto), (override));
    MOCK_METHOD(void, DeleteUser, (const dto::user::DeleteDTO& dto), (override));
    MOCK_METHOD(void, UploadProfilePic, (const boost::uuids::uuid& id, const utils::File& file),
                (override));
    MOCK_METHOD((std::optional<std::pair<std::string, std::string>>), GetProfilePic,
                (const boost::uuids::uuid& id), (override));
};

}  // namespace internview::mocks

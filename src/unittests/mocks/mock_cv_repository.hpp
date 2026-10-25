#pragma once

#include <gmock/gmock.h>

#include "repositories/interfaces/cv_repository.hpp"

namespace internview::mocks {

class MockCvRepository : public repositories::interfaces::ICvRepository {
public:
    MOCK_METHOD(dto::cv::ResponseDTO, CreateCv, (const dto::cv::CreateDTO& dto), (override));
    MOCK_METHOD(std::vector<models::CV>, GetUserCvs, (const boost::uuids::uuid& user_id),
                (override));
    MOCK_METHOD(models::CV, GetCvById,
                (const boost::uuids::uuid& id, const boost::uuids::uuid& user_id), (override));
    MOCK_METHOD(dto::cv::ResponseDTO, UpdateCv, (const dto::cv::UpdateDTO& dto), (override));
    MOCK_METHOD(void, DeleteCv, (const boost::uuids::uuid& id, const boost::uuids::uuid& user_id),
                (override));
    MOCK_METHOD(void, UploadCvPdf,
                (const boost::uuids::uuid& id, const boost::uuids::uuid& user_id,
                 const utils::File& file),
                (override));
    MOCK_METHOD((std::optional<std::pair<std::string, std::string>>), GetCvPdf,
                (const boost::uuids::uuid& id, const boost::uuids::uuid& user_id), (override));
};

}  // namespace internview::mocks

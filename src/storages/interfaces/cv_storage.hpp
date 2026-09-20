#pragma once

#include "models/cv.hpp"
#include "utils/file.hpp"
#include "dto/cv_dto.hpp"

namespace internview::storages::interfaces {

class ICvStorage {
public:
    virtual ~ICvStorage() = default;

    virtual dto::cv::ResponseDTO CreateCv(const dto::cv::CreateDTO& dto) = 0;

    virtual std::vector<internview::models::CV> GetUserCvs(const boost::uuids::uuid& user_id) = 0;

    virtual models::CV GetCvById(const boost::uuids::uuid& id,
                                 const boost::uuids::uuid& user_id) = 0;

    virtual dto::cv::ResponseDTO UpdateCv(const internview::dto::cv::UpdateDTO& dto) = 0;

    virtual void DeleteCv(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) = 0;

    virtual void UploadCvPdf(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id,
                             const utils::File& file) = 0;

    virtual std::optional<std::pair<std::string, std::string>> GetCvPdf(
        const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) = 0;
};

}  // namespace internview::storages::interfaces

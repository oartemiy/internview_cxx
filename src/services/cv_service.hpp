#pragma once

#include <memory>

#include "components/storages/cv_storage_component.hpp"
#include "storages/interfaces/cv_storage.hpp"
#include "userver/components/component_context.hpp"
#include "utils/file.hpp"

namespace internview::services {

class CvService {
public:
    explicit CvService([[maybe_unused]] const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& component_context)
        : cv_storage_(
              component_context.FindComponent<components::CvStorageComponent>().GetStorage()) {
    }

    dto::cv::ResponseDTO CreateCv(const dto::cv::CreateDTO& dto) const {
        return cv_storage_->CreateCv(dto);
    }
    std::vector<internview::models::CV> GetUserCvs(const boost::uuids::uuid& user_id) const {
        return cv_storage_->GetUserCvs(user_id);
    }

    models::CV GetCvById(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) const {
        return cv_storage_->GetCvById(id, user_id);
    }

    dto::cv::ResponseDTO UpdateCv(const internview::dto::cv::UpdateDTO& dto) const {
        return cv_storage_->UpdateCv(dto);
    }

    void DeleteCv(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) const {
        return cv_storage_->DeleteCv(id, user_id);
    }

    void UploadCvPdf(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id,
                     const utils::File& file_arg) const {
        return cv_storage_->UploadCvPdf(id, user_id, file_arg);
    }

    std::optional<std::pair<std::string, std::string>> GetCvPdf(
        const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) const {
        return cv_storage_->GetCvPdf(id, user_id);
    }

private:
    std::shared_ptr<internview::storages::interfaces::ICvStorage> cv_storage_;
};

}  // namespace internview::services

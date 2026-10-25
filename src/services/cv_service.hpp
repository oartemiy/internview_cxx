#pragma once

#include <memory>

#include "components/repositories/cv_repository_component.hpp"
#include "repositories/interfaces/cv_repository.hpp"
#include "userver/components/component_context.hpp"
#include "utils/file.hpp"

namespace internview::services {

class CvService {
public:
    explicit CvService([[maybe_unused]] const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& component_context)
        : cv_repository_(
              component_context.FindComponent<components::CvRepositoryComponent>().GetRepository()) {
    }

    dto::cv::ResponseDTO CreateCv(const dto::cv::CreateDTO& dto) const {
        return cv_repository_->CreateCv(dto);
    }
    std::vector<internview::models::CV> GetUserCvs(const boost::uuids::uuid& user_id) const {
        return cv_repository_->GetUserCvs(user_id);
    }

    models::CV GetCvById(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) const {
        return cv_repository_->GetCvById(id, user_id);
    }

    dto::cv::ResponseDTO UpdateCv(const internview::dto::cv::UpdateDTO& dto) const {
        return cv_repository_->UpdateCv(dto);
    }

    void DeleteCv(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) const {
        return cv_repository_->DeleteCv(id, user_id);
    }

    void UploadCvPdf(const boost::uuids::uuid& id, const boost::uuids::uuid& user_id,
                     const utils::File& file_arg) const {
        return cv_repository_->UploadCvPdf(id, user_id, file_arg);
    }

    std::optional<std::pair<std::string, std::string>> GetCvPdf(
        const boost::uuids::uuid& id, const boost::uuids::uuid& user_id) const {
        return cv_repository_->GetCvPdf(id, user_id);
    }

private:
    std::shared_ptr<internview::repositories::interfaces::ICvRepository> cv_repository_;
};

}  // namespace internview::services

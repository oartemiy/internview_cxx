#pragma once

#include "components/storages/application_storage_component.hpp"
#include "storages/interfaces/application_storage.hpp"
namespace internview::services {

class ApplicationService {
public:
    explicit ApplicationService([[maybe_unused]] const userver::components::ComponentConfig& config,
                                const userver::components::ComponentContext& component_context)
        : application_storage_(
              component_context.FindComponent<components::ApplicationStorageComponent>()
                  .GetStorage()) {
    }

    models::Application CreateApplication(const dto::application::CreateDTO& dto) const {
        return application_storage_->CreateApplication(dto);
    }

    std::vector<models::Application> GetInternsApplications(
        const boost::uuids::uuid& intern_id) const {
        return application_storage_->GetInternsApplications(intern_id);
    }

    std::vector<models::Application> GetRecruiterApplications(
        const boost::uuids::uuid& recruiter_id) const {
        return application_storage_->GetRecruiterApplications(recruiter_id);
    }

    std::vector<models::Application> GetVacancyApplications(
        const boost::uuids::uuid& vacancy_id) const {
        return application_storage_->GetVacancyApplications(vacancy_id);
    }

    models::Application GetApplicationById(const boost::uuids::uuid& id) const {
        return application_storage_->GetApplicationById(id);
    }

    models::Application UpdateApplication(const dto::application::UpdateDTO& dto) const {
        return application_storage_->UpdateApplication(dto);
    }

    void DeleteApplication(const boost::uuids::uuid& id,
                           const boost::uuids::uuid& intern_id) const {
        return application_storage_->DeleteApplication(id, intern_id);
    }

    bool CheckInternApplied(const boost::uuids::uuid& intern_id,
                            const boost::uuids::uuid& recruiter_id) const {
        return application_storage_->CheckInternApplied(intern_id, recruiter_id);
    }

    boost::uuids::uuid GetInternIdByCv(const boost::uuids::uuid& cv_id,
                                       const boost::uuids::uuid& recruiter_id) const {
        return application_storage_->GetInternIdByCv(cv_id, recruiter_id);
    }

private:
    std::shared_ptr<internview::storages::interfaces::IApplicationStorage> application_storage_;
};

}  // namespace internview::services

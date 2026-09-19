#pragma once

#include "components/storages/vacancy_storage_component.hpp"
#include "storages/interfaces/vacancy_storage.hpp"
#include "userver/components/component_context.hpp"
namespace internview::services {

class VacancyService {
public:
    explicit VacancyService([[maybe_unused]] const userver::components::ComponentConfig& config,
                            const userver::components::ComponentContext& component_context)
        : vacancy_storage_(
              component_context.FindComponent<components::VacancyStorageComponent>().GetStorage()) {
    }

    internview::models::Vacancy CreateVacancy(const dto::vacancy::CreateDTO& dto) const {
        return vacancy_storage_->CreateVacancy(dto);
    }

    std::vector<internview::models::Vacancy> GetVacancies(const dto::vacancy::GetDTO& dto) const {
        return vacancy_storage_->GetVacancies(dto);
    }

    internview::models::Vacancy GetVacancyById(const boost::uuids::uuid& id) const {
        return vacancy_storage_->GetVacancyById(id);
    }

    std::vector<internview::models::Vacancy> GetRecruiterVacancies(
        const boost::uuids::uuid& recruiter_id) const {
        return vacancy_storage_->GetRecruiterVacancies(recruiter_id);
    }

    internview::models::Vacancy UpdateVacancy(const dto::vacancy::UpdateDTO& dto) const {
        return vacancy_storage_->UpdateVacancy(dto);
    }

    void DeleteVacancy(const boost::uuids::uuid& id, const boost::uuids::uuid& recruiter_id) const {
        return vacancy_storage_->DeleteVacancy(id, recruiter_id);
    }

    internview::models::Vacancy ToggleVacancy(const boost::uuids::uuid& id,
                                              const boost::uuids::uuid& recruiter_id) const {
        return vacancy_storage_->ToggleVacancy(id, recruiter_id);
    }

    boost::uuids::uuid GetRecruiterIdByApplicationId(
        const boost::uuids::uuid& application_id) const {
        return vacancy_storage_->GetRecruiterIdByApplicationId(application_id);
    }

private:
    std::shared_ptr<internview::storages::interfaces::IVacancyStorage> vacancy_storage_;
};

}  // namespace internview::services

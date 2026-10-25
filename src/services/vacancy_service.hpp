#pragma once

#include "components/repositories/vacancy_repository_component.hpp"
#include "repositories/interfaces/vacancy_repository.hpp"
#include "userver/components/component_context.hpp"
namespace internview::services {

class VacancyService {
public:
    explicit VacancyService([[maybe_unused]] const userver::components::ComponentConfig& config,
                            const userver::components::ComponentContext& component_context)
        : vacancy_repository_(
              component_context.FindComponent<components::VacancyRepositoryComponent>().GetRepository()) {
    }

    internview::models::Vacancy CreateVacancy(const dto::vacancy::CreateDTO& dto) const {
        return vacancy_repository_->CreateVacancy(dto);
    }

    std::vector<internview::models::Vacancy> GetVacancies(const dto::vacancy::GetDTO& dto) const {
        return vacancy_repository_->GetVacancies(dto);
    }

    internview::models::Vacancy GetVacancyById(const boost::uuids::uuid& id) const {
        return vacancy_repository_->GetVacancyById(id);
    }

    std::vector<internview::models::Vacancy> GetRecruiterVacancies(
        const boost::uuids::uuid& recruiter_id) const {
        return vacancy_repository_->GetRecruiterVacancies(recruiter_id);
    }

    internview::models::Vacancy UpdateVacancy(const dto::vacancy::UpdateDTO& dto) const {
        return vacancy_repository_->UpdateVacancy(dto);
    }

    void DeleteVacancy(const boost::uuids::uuid& id, const boost::uuids::uuid& recruiter_id) const {
        return vacancy_repository_->DeleteVacancy(id, recruiter_id);
    }

    internview::models::Vacancy ToggleVacancy(const boost::uuids::uuid& id,
                                              const boost::uuids::uuid& recruiter_id) const {
        return vacancy_repository_->ToggleVacancy(id, recruiter_id);
    }

    boost::uuids::uuid GetRecruiterIdByApplicationId(
        const boost::uuids::uuid& application_id) const {
        return vacancy_repository_->GetRecruiterIdByApplicationId(application_id);
    }

private:
    std::shared_ptr<internview::repositories::interfaces::IVacancyRepository> vacancy_repository_;
};

}  // namespace internview::services

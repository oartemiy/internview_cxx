#pragma once

#include "dto/vacancy_dto.hpp"
#include "models/vacancy.hpp"

namespace internview::storages::interfaces {

class IVacancyStorage {
public:
    virtual ~IVacancyStorage() = default;

    virtual internview::models::Vacancy CreateVacancy(const dto::vacancy::CreateDTO& dto) = 0;

    virtual std::vector<internview::models::Vacancy> GetVacancies(
        const dto::vacancy::GetDTO& dto) = 0;

    virtual internview::models::Vacancy GetVacancyById(const boost::uuids::uuid& id) = 0;

    virtual std::vector<internview::models::Vacancy> GetRecruiterVacancies(
        const boost::uuids::uuid& recruiter_id) = 0;

    virtual internview::models::Vacancy UpdateVacancy(const dto::vacancy::UpdateDTO& dto) = 0;

    virtual void DeleteVacancy(const boost::uuids::uuid& id,
                               const boost::uuids::uuid& recruiter_id) = 0;

    virtual internview::models::Vacancy ToggleVacancy(const boost::uuids::uuid& id,
                                                      const boost::uuids::uuid& recruiter_id) = 0;

    virtual boost::uuids::uuid GetRecruiterIdByApplicationId(
        const boost::uuids::uuid& application_id) = 0;
};

}  // namespace internview::storages::interfaces

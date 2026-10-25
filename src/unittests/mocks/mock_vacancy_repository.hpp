#pragma once

#include <gmock/gmock.h>

#include "repositories/interfaces/vacancy_repository.hpp"

namespace internview::mocks {

class MockVacancyRepository : public repositories::interfaces::IVacancyRepository {
public:
    MOCK_METHOD(models::Vacancy, CreateVacancy, (const dto::vacancy::CreateDTO& dto), (override));
    MOCK_METHOD(std::vector<models::Vacancy>, GetVacancies, (const dto::vacancy::GetDTO& dto),
                (override));
    MOCK_METHOD(models::Vacancy, GetVacancyById, (const boost::uuids::uuid& id), (override));
    MOCK_METHOD(std::vector<models::Vacancy>, GetRecruiterVacancies,
                (const boost::uuids::uuid& recruiter_id), (override));
    MOCK_METHOD(models::Vacancy, UpdateVacancy, (const dto::vacancy::UpdateDTO& dto), (override));
    MOCK_METHOD(void, DeleteVacancy,
                (const boost::uuids::uuid& id, const boost::uuids::uuid& recruiter_id), (override));
    MOCK_METHOD(models::Vacancy, ToggleVacancy,
                (const boost::uuids::uuid& id, const boost::uuids::uuid& recruiter_id), (override));
    MOCK_METHOD(boost::uuids::uuid, GetRecruiterIdByApplicationId,
                (const boost::uuids::uuid& application_id), (override));
};

}  // namespace internview::mocks

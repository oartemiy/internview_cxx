#pragma once

#include <gmock/gmock.h>

#include "repositories/interfaces/application_repository.hpp"

namespace internview::mocks {

class MockApplicationRepository : public repositories::interfaces::IApplicationRepository {
public:
    MOCK_METHOD(models::Application, CreateApplication, (const dto::application::CreateDTO& dto),
                (override));
    MOCK_METHOD(std::vector<models::Application>, GetInternsApplications,
                (const boost::uuids::uuid& intern_id), (override));
    MOCK_METHOD(std::vector<models::Application>, GetRecruiterApplications,
                (const boost::uuids::uuid& recruiter_id), (override));
    MOCK_METHOD(std::vector<models::Application>, GetVacancyApplications,
                (const boost::uuids::uuid& vacancy_id), (override));
    MOCK_METHOD(models::Application, GetApplicationById, (const boost::uuids::uuid& id),
                (override));
    MOCK_METHOD(models::Application, UpdateApplication, (const dto::application::UpdateDTO& dto),
                (override));
    MOCK_METHOD(void, DeleteApplication,
                (const boost::uuids::uuid& id, const boost::uuids::uuid& intern_id), (override));
    MOCK_METHOD(bool, CheckInternApplied,
                (const boost::uuids::uuid& intern_id, const boost::uuids::uuid& recruiter_id),
                (override));
    MOCK_METHOD(boost::uuids::uuid, GetInternIdByCv,
                (const boost::uuids::uuid& cv_id, const boost::uuids::uuid& recruiter_id),
                (override));
};

}  // namespace internview::mocks

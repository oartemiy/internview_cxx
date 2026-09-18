#pragma once

#include "dto/application_dto.hpp"
#include "models/application.hpp"

namespace internview::storages::interfaces {

class IApplicationStorage {
public:
    virtual ~IApplicationStorage() = default;

    virtual models::Application CreateApplication(const dto::application::CreateDTO& dto) = 0;

    virtual std::vector<models::Application> GetInternsApplications(
        const boost::uuids::uuid& intern_id) = 0;

    virtual std::vector<models::Application> GetRecruiterApplications(
        const boost::uuids::uuid& recruiter_id) = 0;

    virtual std::vector<models::Application> GetVacancyApplications(
        const boost::uuids::uuid& vacancy_id) = 0;

    virtual models::Application GetApplicationById(const boost::uuids::uuid& id) = 0;

    virtual models::Application UpdateApplication(const dto::application::UpdateDTO& dto) = 0;

    virtual void DeleteApplication(const boost::uuids::uuid& id,
                                   const boost::uuids::uuid& intern_id) = 0;

    virtual bool CheckInternApplied(const boost::uuids::uuid& intern_id,
                                    const boost::uuids::uuid& recruiter_id) = 0;

    virtual boost::uuids::uuid GetInternIdByCv(const boost::uuids::uuid& cv_id,
                                               const boost::uuids::uuid& recruiter_id) = 0;
};

}  // namespace internview::storages::interfaces

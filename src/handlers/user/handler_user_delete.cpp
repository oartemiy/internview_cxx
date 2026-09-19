#include "handler_user_delete.hpp"

#include "dto/user_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserDelete::HandlerUserDelete(const ComponentConfig& config,
                                     const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_service_(config, component_context),
      cv_service_(config, component_context),
      vacancy_service_(config, component_context) {
}

Value HandlerUserDelete::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                                const Value& request_json,
                                                RequestContext& context) const {
    auto dto = request_json.As<dto::user::DeleteDTO>();
    auto auth_res = context.GetUserData<AuthResult>();
    auto user_id = auth_res.user_id;
    dto.id = user_id;

    if (auth_res.role == "intern") {
        auto user_cvs = cv_service_.GetUserCvs(user_id);
        for (const auto& cv : user_cvs) {
            cv_service_.DeleteCv(cv.id, cv.user_id);
        }
        // NOTE: application deleting is automatic, check schema
    }
    
    if (auth_res.role == "recruiter") {
        auto user_vacancies = vacancy_service_.GetRecruiterVacancies(user_id);
        for (const auto& vacancy : user_vacancies) {
            vacancy_service_.DeleteVacancy(vacancy.id, vacancy.recruiter_id);
        }
    }

    user_service_.DeleteUser(dto);

    return MakeObject("status", "success", "deleted user", dto.login);
}

}  // namespace internview::handlers

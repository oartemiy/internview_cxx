#include "handler_user_delete.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserDelete::HandlerUserDelete(const ComponentConfig& config,
                                     const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerUserDelete::HandleRequestJsonThrow(const HttpRequest& request,
                                                const Value& request_json,
                                                [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::DeleteDTO>();
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    auto user_id = auth_res.user_id;
    dto.id = user_id;

    if (auth_res.role == "intern") {
        auto user_cvs = cv_storage_ptr_->GetUserCvs(user_id);
        for (const auto& cv : user_cvs) {
            cv_storage_ptr_->DeleteCv(cv.id, cv.user_id);
        }
        // NOTE: application deleting is automatic, check schema
    }

    if (auth_res.role == "recruiter") {
        auto user_vacancies = vacancy_storage_ptr_->GetRecruiterVacancies(user_id);
        for (const auto& vacancy : user_vacancies) {
            vacancy_storage_ptr_->DeleteVacancy(vacancy.id, vacancy.recruiter_id);
        }
    }

    // LOG_INFO() << token;
    user_storage_ptr_->DeleteUser(dto);

    return MakeObject("status", "success", "deleted user", dto.login);
}

}  // namespace internview::handlers

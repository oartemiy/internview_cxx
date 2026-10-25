#include "handler_vacancies_update_id.hpp"

#include "dto/vacancy_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyUpdate::HandlerVacancyUpdate(const ComponentConfig& config,
                                           const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), vacancy_service_(config, component_context) {
}

Value HandlerVacancyUpdate::HandleRequestJsonThrow(const HttpRequest& request,
                                                   const Value& request_json,
                                                   [[maybe_unused]] RequestContext& context) const {
    if (request_json.IsEmpty()) {
        throw ClientError(MakeObject("message", "Empty json"));
    }
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "recruiter");
    auto id = ParseUuidPathArg(request, "id");

    auto dto = ParseJsonBody<dto::vacancy::UpdateDTO>(request_json);
    if (dto.has_title_in_request_json) {
        ValidateLength("title", dto.title, 1, 200);
    }
    ValidateEnum("work_mode", dto.work_mode, {"remote", "office", "hybrid"});
    ValidateEnum("experience_level", dto.experience_level, {"junior", "middle", "senior"});
    dto.id = id;
    dto.recruiter_id = auth_res.user_id;

    if (vacancy_service_.GetVacancyById(id).recruiter_id != auth_res.user_id) {
        throw Forbidden(MakeObject("message", "This vacancy does not belongs to you"));
    }
    auto updated_model = vacancy_service_.UpdateVacancy(dto);

    return ValueBuilder(updated_model).ExtractValue();
}

}  // namespace internview::handlers

#include "handler_vacancies_delete.hpp"

#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacanciesDelete::HandlerVacanciesDelete(const ComponentConfig& config,
                                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), vacancy_service_(config, component_context) {
}

Value HandlerVacanciesDelete::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "recruiter");
    auto id = ParseUuidPathArg(request, "id");

    if (vacancy_service_.GetVacancyById(id).recruiter_id != auth_res.user_id) {
        throw Forbidden(MakeObject("message", "This vacancy does not belongs to you"));
    }
    vacancy_service_.DeleteVacancy(id, auth_res.user_id);

    request.SetResponseStatus(HttpStatus::kNoContent);
    return {};
}

}  // namespace internview::handlers

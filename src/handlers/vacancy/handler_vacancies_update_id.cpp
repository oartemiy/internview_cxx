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

    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }

    auto dto = request_json.As<dto::vacancy::UpdateDTO>();
    dto.id = id;
    dto.recruiter_id = auth_res.user_id;

    auto updated_model = vacancy_service_.UpdateVacancy(dto);

    return ValueBuilder(updated_model).ExtractValue();
}

}  // namespace internview::handlers

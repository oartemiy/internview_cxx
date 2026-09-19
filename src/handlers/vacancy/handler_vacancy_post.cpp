#include "handler_vacancy_post.hpp"

#include "dto/vacancy_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyPost::HandlerVacancyPost(const ComponentConfig& config,
                                       const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), vacancy_service_(config, component_context) {
}

Value HandlerVacancyPost::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                                 const Value& request_json,
                                                 [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }
    auto dto = request_json.As<dto::vacancy::CreateDTO>();
    dto.recruiter_id = auth_res.user_id;

    auto model = vacancy_service_.CreateVacancy(dto);

    return ValueBuilder(model).ExtractValue();
}

}  // namespace internview::handlers

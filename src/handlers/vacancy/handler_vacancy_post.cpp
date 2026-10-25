#include "handler_vacancy_post.hpp"

#include "dto/vacancy_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyPost::HandlerVacancyPost(const ComponentConfig& config,
                                       const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), vacancy_service_(config, component_context) {
}

Value HandlerVacancyPost::HandleRequestJsonThrow(const HttpRequest& request,
                                                 const Value& request_json,
                                                 [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "recruiter");
    auto dto = ParseJsonBody<dto::vacancy::CreateDTO>(request_json);
    ValidateLength("title", dto.title, 1, 200);
    ValidateEnum("work_mode", dto.work_mode, {"remote", "office", "hybrid"});
    ValidateEnum("experience_level", dto.experience_level, {"junior", "middle", "senior"});
    dto.recruiter_id = auth_res.user_id;

    auto model = vacancy_service_.CreateVacancy(dto);

    request.SetResponseStatus(HttpStatus::kCreated);
    return ValueBuilder(model).ExtractValue();
}

}  // namespace internview::handlers

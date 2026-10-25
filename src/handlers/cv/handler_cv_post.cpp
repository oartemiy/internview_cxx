#include "handler_cv_post.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvPost::HandlerCvPost(const ComponentConfig& config,
                             const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), cv_service_(config, component_context) {
}

Value HandlerCvPost::HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                            [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "intern");

    auto dto = ParseJsonBody<internview::dto::cv::CreateDTO>(request_json);
    ValidateLength("title", dto.title, 1, 200);
    dto.user_id = auth_res.user_id;
    auto res = cv_service_.CreateCv(dto);

    request.SetResponseStatus(HttpStatus::kCreated);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

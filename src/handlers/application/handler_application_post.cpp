#include "handler_application_post.hpp"

#include "dto/application_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationPost::HandlerApplicationPost(const ComponentConfig& config,
                                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_service_(config, component_context) {
}

Value HandlerApplicationPost::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {

    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }

    auto dto = request_json.As<dto::application::CreateDTO>();
    dto.intern_id = auth_res.user_id;

    auto model = application_service_.CreateApplication(dto);

    return ValueBuilder(model).ExtractValue();
}

}  // namespace internview::handlers

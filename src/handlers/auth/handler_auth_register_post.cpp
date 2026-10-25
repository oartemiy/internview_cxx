#include "handler_auth_register_post.hpp"

#include "components/services/auth_service_component.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthRegisterPost::HandlerAuthRegisterPost(const ComponentConfig& config,
                                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_(
          component_context.FindComponent<components::AuthServiceComponent>().GetService()) {
}

Value HandlerAuthRegisterPost::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    dto::user::RegisterDTO dto = ParseJsonBody<dto::user::RegisterDTO>(request_json);
    if (dto.role != "intern" && dto.role != "recruiter") {
        throw ClientError(MakeObject("message", "Available roles: intern, recruiter"));
    }
    if (dto.login == "me") {
        throw ClientError(MakeObject("message", "Login: me can not be taken"));
    }
    ValidateLength("login", dto.login, 3, 50);
    ValidateLength("name", dto.name, 1, 100);
    ValidateLength("password", dto.password, 6, 128);
    auto res = auth_service_->Register(dto);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

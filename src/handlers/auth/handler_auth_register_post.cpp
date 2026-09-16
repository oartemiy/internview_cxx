#include "handler_auth_register_post.hpp"

#include "components/internview_component.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthRegisterPost::HandlerAuthRegisterPost(const ComponentConfig& config,
                                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerAuthRegisterPost::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    dto::user::CreateDTO dto = request_json.As<dto::user::CreateDTO>();
    if (dto.role != "intern" && dto.role != "recruiter") {
        throw userver::server::handlers::ClientError(
            MakeObject("message", "Available roles: intern, recruiter"));
    }
    if (dto.login == "me") {
        throw userver::server::handlers::ClientError(
            MakeObject("message", "Login: me can not be taken"));
    }
    if (dto.password.length() <= 1) {
        throw userver::server::handlers::ClientError(
            MakeObject("message", "Password must be at least 2 chars"));
    }
    auto res = auth_service_ptr_->Register(dto);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

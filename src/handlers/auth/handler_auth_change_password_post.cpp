#include "handler_auth_change_password_post.hpp"

#include "components/services/auth_service_component.hpp"
#include "dto/user_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthChangePasswordPost::HandlerAuthChangePasswordPost(
    const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_(
          component_context.FindComponent<components::AuthServiceComponent>().GetService()) {
}

Value HandlerAuthChangePasswordPost::HandleRequestJsonThrow(const HttpRequest& request,
                                                            const Value& request_json,
                                                            RequestContext& context) const {
    auto dto = ParseJsonBody<dto::user::ChangePasswordDTO>(request_json);
    ValidateLength("new_password", dto.new_password, 6, 128);
    auto auth_res = context.GetUserData<AuthResult>();
    auto user_id = auth_res.user_id;
    dto.id = user_id;

    auth_service_->ChangePassword(dto);

    request.SetResponseStatus(HttpStatus::kNoContent);
    return {};
}

}  // namespace internview::handlers

#include "handler_auth_change_password_post.hpp"

#include "dto/user_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthChangePasswordPost::HandlerAuthChangePasswordPost(
    const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerAuthChangePasswordPost::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, const Value& request_json,
    RequestContext& context) const {
    auto dto = request_json.As<dto::user::ChangePasswordDTO>();
    auto auth_res = context.GetUserData<AuthResult>();
    auto user_id = auth_res.user_id;
    dto.id = user_id;

    auth_service_ptr_->ChangePassword(dto);

    return MakeObject("status", "password changed");
}

}  // namespace internview::handlers

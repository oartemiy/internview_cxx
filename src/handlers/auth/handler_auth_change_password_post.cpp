#include "handler_auth_change_password_post.hpp"

#include "dto/user_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthChangePasswordPost::HandlerAuthChangePasswordPost(
    const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerAuthChangePasswordPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::ChangePasswordDTO>();
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    auto user_id = auth_res.user_id;
    dto.id = user_id;

    user_storage_ptr_->ChangeUserPassword(dto);

    return MakeObject("status", "password changed");
}

}  // namespace internview::handlers

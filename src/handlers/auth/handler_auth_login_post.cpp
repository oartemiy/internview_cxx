#include "handler_auth_login_post.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerAuthLoginPost::HandlerAuthLoginPost(const ComponentConfig& config,
                                           const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerAuthLoginPost::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                                   const Value& request_json,
                                                   [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::LoginDTO>();
    auto responce = user_storage_ptr_->LoginUser(dto);
    return ValueBuilder(responce).ExtractValue();
}

}  // namespace internview::handlers

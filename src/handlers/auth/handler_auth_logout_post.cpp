#include "handler_auth_logout_post.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthLogoutPost::HandlerAuthLogoutPost(const ComponentConfig& config,
                                             const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

std::string HandlerAuthLogoutPost::HandleRequestThrow([[maybe_unused]] const HttpRequest& request,
                                                      RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    auth_service_ptr_->Revoke(auth_res.user_id);
    return "All sessions are finished";
}

}  // namespace internview::handlers

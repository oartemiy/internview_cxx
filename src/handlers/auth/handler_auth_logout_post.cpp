#include "handler_auth_logout_post.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthLogoutPost::HandlerAuthLogoutPost(const ComponentConfig& config,
                                             const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

std::string HandlerAuthLogoutPost::HandleRequestThrow(
    const HttpRequest& request, [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    auth_service_ptr_->RevokeRefreshTokens(auth_res.user_id);
    return "All sessions are finished";
}

}  // namespace internview::handlers

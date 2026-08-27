#include "handler_auth_refresh_post.hpp"

#include <string>

#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthRefreshPost::HandlerAuthRefreshPost(const ComponentConfig& config,
                                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerAuthRefreshPost::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto refresh_token = request_json["refresh_token"].As<std::string>();
    auto new_tokens = auth_service_ptr_->RefreshTokens(refresh_token);
    return MakeObject("refresh_token", new_tokens.refresh_token, "access_token",
                      new_tokens.access_token);
}
}  // namespace internview::handlers

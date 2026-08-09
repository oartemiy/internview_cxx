#include "handler_auth_change_password_post.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/server/http/http_status.hpp"

namespace internview::handlers {

HandlerAuthChangePasswordPost::HandlerAuthChangePasswordPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

HandlerAuthChangePasswordPost::Value HandlerAuthChangePasswordPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::ChangePasswordDTO>();
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.substr(7).empty()) {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "unauthorized";
        request.GetHttpResponse().SetStatus(userver::http::kUnauthorized);
        return builder.ExtractValue();
    }
    auto token = auth_header.substr(7);
    auto is_changed = user_storage_ref_.ChangeUserPassword(token, dto);
    if (!is_changed) {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "invalid password or expired jwt token";
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::BadRequest);
        return builder.ExtractValue();
    }
    userver::formats::json::ValueBuilder builder;
    builder["status"] = "password changed";
    return builder.ExtractValue();
}

}  // namespace internview::handlers

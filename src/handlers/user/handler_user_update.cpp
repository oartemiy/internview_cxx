#include "handler_user_update.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerUserUpdate::HandlerUserUpdate(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

HandlerUserUpdate::Value HandlerUserUpdate::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<internview::dto::user::UpdateDTO>();
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.substr(7).empty()) {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "unauthorized";
        request.GetHttpResponse().SetStatus(userver::http::kUnauthorized);
        return builder.ExtractValue();
    }
    auto token = auth_header.substr(7);
    auto res = user_storage_ref_.UpdateUser(token, dto);
    if (res) {
        userver::formats::json::ValueBuilder builder;
        builder["status"] = "updated";
        builder["updated_user"] = dto.login;
        return builder.ExtractValue();
    } else {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "invalid request data";
        request.GetHttpResponse().SetStatusNotFound();
        return builder.ExtractValue();
    }
}

}  // namespace internview::handlers

#include "handler_user_get.hpp"

#include "components/internview_component.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerUserGet::HandlerUserGet(const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

HandlerUserGet::Value HandlerUserGet::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.substr(7).empty()) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kUnauthorized);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "unauthorized";
        return builder.ExtractValue();
    }
    auto token = auth_header.substr(7);
    auto res = user_storage_ref_.GetUserByJWT(token);
    if (res) {
        auto json = userver::formats::json::ValueBuilder(*res).ExtractValue();
        return json;
    } else {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "invalid request data";
        request.GetHttpResponse().SetStatusNotFound();
        return builder.ExtractValue();
    }
}

}  // namespace internview::handlers

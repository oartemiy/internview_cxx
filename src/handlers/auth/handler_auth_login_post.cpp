#include "handler_auth_login_post.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "storages/user_storage.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerAuthLoginPost::HandlerAuthLoginPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

HandlerAuthLoginPost::Value HandlerAuthLoginPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::LoginDTO>();
    auto responce = user_storage_ref_.LoginUser(dto);
    if (!responce) {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Invalid login or password. Try again";
        request.GetHttpResponse().SetStatus(userver::http::kBadRequest);
        return builder.ExtractValue();
    }
    auto json = userver::formats::json::ValueBuilder(*responce).ExtractValue();
    return json;
}

}  // namespace internview::handlers

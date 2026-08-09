#include "handler_user_post.hpp"

#include <optional>
#include <string>

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/server/http/http_status.hpp"

namespace internview::handlers {

HandlerUserPost::HandlerUserPost(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ref_(
          component_context
              .FindComponent<internview::components::InternviewComponent>("internview-component")
              .GetUserStorageRef()) {
}

HandlerUserPost::Value HandlerUserPost::HandleRequestJsonThrow(
    [[maybe_unused]] const userver::server::http::HttpRequest& request, const Value& request_json,
    [[maybe_unused]] userver::server::request::RequestContext& context) const {
    dto::user::CreateDTO dto = request_json.As<dto::user::CreateDTO>();
    auto res = user_storage_ref_.CreateUser(dto);
    if (!res) {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "login: " + dto.login + " is taken, try another one";
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::BadRequest);
        return builder.ExtractValue();
    }
    auto response_json = userver::formats::json::ValueBuilder(*res).ExtractValue();
    return response_json;
}
}  // namespace internview::handlers

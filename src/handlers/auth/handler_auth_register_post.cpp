#include "handler_auth_register_post.hpp"

#include "components/internview_component.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerAuthRegisterPost::HandlerAuthRegisterPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

HandlerAuthRegisterPost::Value HandlerAuthRegisterPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    dto::user::CreateDTO dto = request_json.As<dto::user::CreateDTO>();
    if (dto.login == "me") {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "login: \"me\" can not be taken";
        request.GetHttpResponse().SetStatus(userver::http::kBadRequest);
        return builder.ExtractValue();
    }
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

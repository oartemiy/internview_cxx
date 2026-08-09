#include "handler_user_delete.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/server/http/http_status.hpp"

namespace internview::handlers {

HandlerUserDelete::HandlerUserDelete(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_(component_context.FindComponent<internview::components::InternviewComponent>()
                        .GetUserStorageRef()) {
}

HandlerUserDelete::Value HandlerUserDelete::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::DeleteDTO>();
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.substr(7).empty()) {
        request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kUnauthorized);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "unauthorized";
        return builder.ExtractValue();
    }
    // LOG_INFO() << auth_header;
    auto token = auth_header.substr(7);
    // LOG_INFO() << token;
    auto res = user_storage_.DeleteUser(token, dto);
    if (res) {
        userver::formats::json::ValueBuilder builder;
        builder["status"] = "deleted";
        builder["deleted_user"] = dto.login;
        return builder.ExtractValue();
    } else {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "invalid request data";
        request.GetHttpResponse().SetStatusNotFound();
        return builder.ExtractValue();
    }
}

}  // namespace internview::handlers

#include "handler_auth_register_post.hpp"

#include "components/internview_component.hpp"
#include "errors/errors.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerAuthRegisterPost::HandlerAuthRegisterPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetAuthServicePtr()) {
}

HandlerAuthRegisterPost::Value HandlerAuthRegisterPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    dto::user::CreateDTO dto = request_json.As<dto::user::CreateDTO>();
    if (dto.login == "me") {
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "login: me can not be taken";
        request.GetHttpResponse().SetStatus(userver::http::kBadRequest);
        return builder.ExtractValue();
    }
    try {
        auto res = user_storage_ptr_->CreateUser(dto);
        return userver::formats::json::ValueBuilder(res).ExtractValue();
    } catch (errors::ConflictError& e) {
        userver::formats::json::ValueBuilder builder;
        request.SetResponseStatus(userver::http::kConflict);
        builder["error"] = e.what();
        return builder.ExtractValue();
    }
}

}  // namespace internview::handlers

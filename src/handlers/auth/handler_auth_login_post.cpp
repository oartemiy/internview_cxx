#include "handler_auth_login_post.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "storages/user_storage.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "errors/errors.hpp"

namespace internview::handlers {

HandlerAuthLoginPost::HandlerAuthLoginPost(
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

HandlerAuthLoginPost::Value HandlerAuthLoginPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::LoginDTO>();
    try {
        auto responce = user_storage_ptr_->LoginUser(dto);
        return userver::formats::json::ValueBuilder(responce).ExtractValue();
    } catch (errors::InvalidPasswordError& e) {
        userver::formats::json::ValueBuilder builder;
        request.SetResponseStatus(userver::http::kBadRequest);
        builder["error"] = e.what();
        return builder.ExtractValue();
    } catch (errors::NotFoundError& e) {
        userver::formats::json::ValueBuilder builder;
        request.SetResponseStatus(userver::http::kNotFound);
        builder["error"] = e.what();
        return builder.ExtractValue();
    }
}

}  // namespace internview::handlers

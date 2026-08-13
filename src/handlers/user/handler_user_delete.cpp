#include "handler_user_delete.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "errors/errors.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerUserDelete::HandlerUserDelete(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetAuthServicePtr()) {
}

HandlerUserDelete::Value HandlerUserDelete::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::DeleteDTO>();
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->IsAuthorized(auth_header);
    if (!auth_res) {
        request.SetResponseStatus(userver::http::kUnauthorized);

        return services::GetJSON(auth_res);

    }
    auto user_id = *auth_res;
    dto.user_id = user_id;
    // LOG_INFO() << token;
    try {
        user_storage_ptr_->DeleteUser(dto);
        userver::formats::json::ValueBuilder builder;
        builder["status"] = "success";
        builder["deleted user"] = dto.login;
        return builder.ExtractValue();
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

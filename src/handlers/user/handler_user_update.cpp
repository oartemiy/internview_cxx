#include "handler_user_update.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "errors/errors.hpp"
#include "services/auth_service.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerUserUpdate::HandlerUserUpdate(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetAuthServicePtr()) {
}

HandlerUserUpdate::Value HandlerUserUpdate::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<internview::dto::user::UpdateDTO>();
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->IsAuthorized(auth_header);
    if (!auth_res) {
        request.SetResponseStatus(userver::http::kUnauthorized);

        return services::GetJSON(auth_res);
    }
    dto.user_id = *auth_res;
    try {
        auto res = user_storage_ptr_->UpdateUser(dto);
        return userver::formats::json::ValueBuilder(res).ExtractValue();

    } catch (internview::errors::ConflictError& e) {
        request.SetResponseStatus(userver::http::kConflict);
        return userver::formats::json::MakeObject("error", e.what());
    } catch (internview::errors::NotFoundError& e) {
        request.SetResponseStatus(userver::http::kNotFound);
        return userver::formats::json::MakeObject("error", e.what());
    }
}

}  // namespace internview::handlers

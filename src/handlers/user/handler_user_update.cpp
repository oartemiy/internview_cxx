#include "handler_user_update.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "services/auth_service.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserUpdate::HandlerUserUpdate(const ComponentConfig& config,
                                     const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()) {
}

Value HandlerUserUpdate::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                                const Value& request_json,
                                                [[maybe_unused]] RequestContext& context) const {
    if (request_json.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            MakeObject("message", "Empty request data body. Nothing to update"));
    }
    auto dto = request_json.As<internview::dto::user::UpdateDTO>();
    auto auth_res = context.GetUserData<AuthResult>();

    dto.id = auth_res.user_id;

    if (dto.login == "me") {
        throw userver::server::handlers::ClientError(
            MakeObject("message", "Login: me can not be taken"));
    }
    auto res = user_storage_ptr_->UpdateUser(dto);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

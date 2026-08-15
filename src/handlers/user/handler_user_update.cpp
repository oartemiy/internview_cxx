#include "handler_user_update.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "services/auth_service.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerUserUpdate::HandlerUserUpdate(const ComponentConfig& config,
                                     const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerUserUpdate::HandleRequestJsonThrow(const HttpRequest& request,
                                                const Value& request_json,
                                                [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<internview::dto::user::UpdateDTO>();
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    dto.id = auth_res.user_id;

    auto res = user_storage_ptr_->UpdateUser(dto);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

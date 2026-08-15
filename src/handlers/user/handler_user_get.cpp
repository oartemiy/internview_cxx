#include "handler_user_get.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerUserGet::HandlerUserGet(const ComponentConfig& config,
                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerUserGet::HandleRequestJsonThrow(const HttpRequest& request,
                                             [[maybe_unused]] const Value& request_json,
                                             [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    auto user_id = auth_res.user_id;
    auto user = user_storage_ptr_->GetUserById(user_id);
    auto resp_dto =
        dto::user::ResponseDTO(user.id, user.login, user.name, user.role, user.description,
                               user.profile_pic, user.created_at, std::nullopt);
    return ValueBuilder(resp_dto).ExtractValue();
}

}  // namespace internview::handlers

#include "handler_user_delete.hpp"

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserDelete::HandlerUserDelete(const ComponentConfig& config,
                                     const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerUserDelete::HandleRequestJsonThrow(const HttpRequest& request,
                                                const Value& request_json,
                                                [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<dto::user::DeleteDTO>();
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    auto user_id = auth_res.user_id;
    dto.user_id = user_id;
    // LOG_INFO() << token;
    user_storage_ptr_->DeleteUser(dto);

    return MakeObject("status", "success", "deleted user", dto.login);
}

}  // namespace internview::handlers

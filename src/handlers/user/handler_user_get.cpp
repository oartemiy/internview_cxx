#include "handler_user_get.hpp"

#include "dto/user_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserGet::HandlerUserGet(const ComponentConfig& config,
                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), user_service_(config, component_context) {
}

Value HandlerUserGet::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                             [[maybe_unused]] const Value& request_json,
                                             [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    auto user_id = auth_res.user_id;
    auto user = user_service_.GetUserById(user_id);
    auto resp_dto =
        dto::user::ResponseDTO(user.id, user.login, user.name, user.role, user.description,
                               user.profile_pic, user.created_at, std::nullopt);
    return ValueBuilder(resp_dto).ExtractValue();
}

}  // namespace internview::handlers

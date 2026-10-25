#include "handler_user_update.hpp"

#include "dto/user_dto.hpp"
#include "services/auth_service.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserUpdate::HandlerUserUpdate(const ComponentConfig& config,
                                     const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), user_service_(config, component_context) {
}

Value HandlerUserUpdate::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                                const Value& request_json,
                                                [[maybe_unused]] RequestContext& context) const {
    if (request_json.IsEmpty()) {
        throw ClientError(MakeObject("message", "Empty request data body. Nothing to update"));
    }
    auto dto = ParseJsonBody<internview::dto::user::UpdateDTO>(request_json);
    auto auth_res = context.GetUserData<AuthResult>();

    dto.id = auth_res.user_id;

    if (dto.has_login_in_request) {
        if (dto.login == "me") {
            throw ClientError(MakeObject("message", "Login: me can not be taken"));
        }
        ValidateLength("login", dto.login, 3, 50);
    }
    if (dto.has_name_in_request) {
        ValidateLength("name", dto.name, 1, 100);
    }
    // NOTE: profile_pic is a file storage key, it can only be removed here (set to null)
    if (dto.has_profile_pic_in_request && dto.profile_pic) {
        throw ClientError(MakeObject(
            "message", "profile_pic can only be set to null, use POST /users/me/profile-pic"));
    }
    auto res = user_service_.UpdateUser(dto);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

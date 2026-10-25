#include "handler_user_get_by_id.hpp"

#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserGetById::HandlerUserGetById(const ComponentConfig& config,
                                       const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_service_(config, component_context),
      user_service_(config, component_context) {
}

Value HandlerUserGetById::HandleRequestJsonThrow(const HttpRequest& request,
                                                 [[maybe_unused]] const Value& request_json,
                                                 [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    auto id = ParseUuidPathArg(request, "id");
    if (auth_res.user_id == id || auth_res.role == "recruiter") {
        if (auth_res.user_id != id &&
            !application_service_.CheckInternApplied(id, auth_res.user_id)) {
            throw Forbidden(MakeObject("message", "You do not have roots to do this action"));
        }
        auto res = user_service_.GetUserById(id).ToResponseDTO();
        return ValueBuilder(res).ExtractValue();
    } else {
        throw Forbidden(MakeObject("message", "Invalid role or id for this action"));
    }
}

}  // namespace internview::handlers

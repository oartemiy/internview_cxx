#include "handler_application_delete.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationDelete::HandlerApplicationDelete(const ComponentConfig& config,
                                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_service_(config, component_context) {
}

Value HandlerApplicationDelete::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "intern");
    auto id = ParseUuidPathArg(request, "id");
    if (application_service_.GetApplicationById(id).intern_id != auth_res.user_id) {
        throw Forbidden(MakeObject("message", "This application does not belongs to you"));
    }
    application_service_.DeleteApplication(id, auth_res.user_id);
    request.SetResponseStatus(HttpStatus::kNoContent);
    return {};
}

}  // namespace internview::handlers

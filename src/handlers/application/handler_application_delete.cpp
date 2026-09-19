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
    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    application_service_.DeleteApplication(id, auth_res.user_id);
    return MakeObject("status", "deleted", "deleted application", request.GetPathArg("id"));
}

}  // namespace internview::handlers

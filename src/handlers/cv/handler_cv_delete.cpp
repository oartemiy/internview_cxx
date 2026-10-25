#include "handler_cv_delete.hpp"

#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvDelete::HandlerCvDelete(const ComponentConfig& config,
                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), cv_service_(config, component_context) {
}

Value HandlerCvDelete::HandleRequestJsonThrow(const HttpRequest& request,
                                              [[maybe_unused]] const Value& request_json,
                                              [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "intern");

    auto id = ParseUuidPathArg(request, "id");
    cv_service_.DeleteCv(id, auth_res.user_id);
    request.SetResponseStatus(HttpStatus::kNoContent);
    return {};
}

}  // namespace internview::handlers

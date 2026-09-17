#include "handler_cv_delete.hpp"

#include "components/cv_storage_component.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvDelete::HandlerCvDelete(const ComponentConfig& config,
                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      cv_storage_ptr_(component_context.FindComponent<components::CvStorageComponent>().GetStorage()) {
}

Value HandlerCvDelete::HandleRequestJsonThrow(const HttpRequest& request,
                                              [[maybe_unused]] const Value& request_json,
                                              [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }

    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    cv_storage_ptr_->DeleteCv(id, auth_res.user_id);
    return MakeObject("status", "success", "deleted cv", request.GetPathArg("id"));
}

}  // namespace internview::handlers

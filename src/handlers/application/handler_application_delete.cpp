#include "handler_application_delete.hpp"

#include "components/internview_component.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationDelete::HandlerApplicationDelete(const ComponentConfig& config,
                                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetApplicationStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerApplicationDelete::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    application_storage_ptr_->DeleteApplication(id, auth_res.user_id);
    return MakeObject("status", "deleted", "deleted application", request.GetPathArg("id"));
}

}  // namespace internview::handlers

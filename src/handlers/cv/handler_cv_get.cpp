#include "handler_cv_get.hpp"

#include "components/internview_component.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvGet::HandlerCvGet(const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerCvGet::HandleRequestJsonThrow(const HttpRequest& request,
                                           [[maybe_unused]] const Value& request_json,
                                           [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }

    auto resp_vec = cv_storage_ptr_->GetUserCvs(auth_res.user_id);
    return ValueBuilder(resp_vec).ExtractValue();
}

}  // namespace internview::handlers

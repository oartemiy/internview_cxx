#include "handler_cv_get.hpp"

#include "components/internview_component.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvGet::HandlerCvGet(const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()) {
}

Value HandlerCvGet::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                           [[maybe_unused]] const Value& request_json,
                                           [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }

    auto resp_vec = cv_storage_ptr_->GetUserCvs(auth_res.user_id);
    return ValueBuilder(resp_vec).ExtractValue();
}

}  // namespace internview::handlers

#include "handler_applications_get.hpp"

#include "components/application_storage_component.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationsGet::HandlerApplicationsGet(const ComponentConfig& config,
                                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_storage_ptr_(
          component_context.FindComponent<components::ApplicationStorageComponent>().GetStorage()) {
}

Value HandlerApplicationsGet::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {

    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role == "intern") {
        auto res_vec = application_storage_ptr_->GetInternsApplications(auth_res.user_id);
        return ValueBuilder(res_vec).ExtractValue();
    } else {
        auto res_vec = application_storage_ptr_->GetRecruiterApplications(auth_res.user_id);
        return ValueBuilder(res_vec).ExtractValue();
    }
}

}  // namespace internview::handlers

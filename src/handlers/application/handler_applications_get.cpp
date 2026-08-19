#include "handler_applications_get.hpp"

#include "components/internview_component.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationsGet::HandlerApplicationsGet(const ComponentConfig& config,
                                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetApplicationStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerApplicationsGet::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    if (auth_res.role == "intern") {
        auto res_vec = application_storage_ptr_->GetInternsApplications(auth_res.user_id);
        return ValueBuilder(res_vec).ExtractValue();
    } else {
        auto res_vec = application_storage_ptr_->GetRecruiterApplications(auth_res.user_id);
        return ValueBuilder(res_vec).ExtractValue();
    }
}

}  // namespace internview::handlers

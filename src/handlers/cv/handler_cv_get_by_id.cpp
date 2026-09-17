#include "handler_cv_get_by_id.hpp"

#include "components/cv_storage_component.hpp"
#include "components/internview_component.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvGetById::HandlerCvGetById(const ComponentConfig& config,
                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_storage_(
          component_context.FindComponent<InternviewComponent>().GetApplicationStoragePtr()),
      cv_storage_ptr_(
          component_context.FindComponent<components::CvStorageComponent>().GetStorage()) {
}

Value HandlerCvGetById::HandleRequestJsonThrow(const HttpRequest& request,
                                               [[maybe_unused]] const Value& request_json,
                                               [[maybe_unused]] RequestContext& context) const {
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    auto auth_res = context.GetUserData<AuthResult>();
    auto intern_id = auth_res.user_id;

    if (auth_res.role == "recruiter") {
        intern_id = application_storage_->GetInternIdByCv(id, auth_res.user_id);
    }

    auto resp_model = cv_storage_ptr_->GetCvById(id, intern_id);
    return ValueBuilder(resp_model).ExtractValue();
}

}  // namespace internview::handlers

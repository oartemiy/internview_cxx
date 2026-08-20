#include "handler_vacancy_applications_get.hpp"

#include "components/internview_component.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyApplicationsGet::HandlerVacancyApplicationsGet(
    const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()),
      application_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetApplicationStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerVacancyApplicationsGet::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    if (vacancy_storage_ptr_->GetVacancyById(id).recruiter_id == auth_res.user_id) {
        auto vec = application_storage_ptr_->GetVacancyApplications(id);
        return ValueBuilder(vec).ExtractValue();
    } else {
        throw ClientError(
            MakeObject("message", "this vacancy does not belongs to you. Check vacancy id"));
    }
}

}  // namespace internview::handlers

#include "handler_vacancy_applications_get.hpp"

#include "components/application_storage_component.hpp"
#include "components/internview_component.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyApplicationsGet::HandlerVacancyApplicationsGet(
    const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()),
      application_storage_ptr_(
          component_context.FindComponent<components::ApplicationStorageComponent>().GetStorage()) {
}

Value HandlerVacancyApplicationsGet::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {

    auto auth_res = context.GetUserData<AuthResult>();
    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    if (vacancy_storage_ptr_->GetVacancyById(id).recruiter_id == auth_res.user_id) {
        auto vec = application_storage_ptr_->GetVacancyApplications(id);
        return ValueBuilder(vec).ExtractValue();
    } else {
        throw ClientError(
            MakeObject("message", "This vacancy does not belongs to you. Check vacancy id"));
    }
}

}  // namespace internview::handlers

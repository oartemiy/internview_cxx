#include "handler_vacancies_toggle.hpp"

#include "components/internview_component.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyToggle::HandlerVacancyToggle(const ComponentConfig& config,
                                           const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()) {
}

Value HandlerVacancyToggle::HandleRequestJsonThrow(const HttpRequest& request,
                                                   [[maybe_unused]] const Value& request_json,
                                                   [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));

    auto model = vacancy_storage_ptr_->ToggleVacancy(id, auth_res.user_id);
    return ValueBuilder(model).ExtractValue();
}
}  // namespace internview::handlers

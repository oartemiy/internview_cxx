#include "handler_vacancies_get_me.hpp"

#include "components/internview_component.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacanciesGetMe::HandlerVacanciesGetMe(const ComponentConfig& config,
                                             const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()) {
}

Value HandlerVacanciesGetMe::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }
    auto res = vacancy_storage_ptr_->GetRecruiterVacancies(auth_res.user_id);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

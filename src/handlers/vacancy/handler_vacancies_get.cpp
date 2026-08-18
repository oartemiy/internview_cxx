#include "handler_vacancies_get.hpp"

#include <cstddef>
#include <string>

#include "components/internview_component.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacanciesGet::HandlerVacanciesGet(const ComponentConfig& config,
                                         const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()) {
}

Value HandlerVacanciesGet::HandleRequestJsonThrow(const HttpRequest& request,
                                                  [[maybe_unused]] const Value& request_json,
                                                  [[maybe_unused]] RequestContext& context) const {
    int offset = 0;
    int limit = 15;
    if (request.HasArg("limit")) {
        limit = std::stoi(request.GetArg("limit"));
    }
    if (request.HasArg("offset")) {
        offset = std::stoi(request.GetArg("offset"));
    }
    auto res = vacancy_storage_ptr_->GetVacancies(limit, offset);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

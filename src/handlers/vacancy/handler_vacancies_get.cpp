#include "handler_vacancies_get.hpp"

#include <cstddef>
#include <string>

#include "components/vacancy_storage_component.hpp"
#include "dto/vacancy_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacanciesGet::HandlerVacanciesGet(const ComponentConfig& config,
                                         const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      vacancy_storage_ptr_(
          component_context.FindComponent<components::VacancyStorageComponent>().GetStorage()) {
}

Value HandlerVacanciesGet::HandleRequestJsonThrow(const HttpRequest& request,
                                                  [[maybe_unused]] const Value& request_json,
                                                  [[maybe_unused]] RequestContext& context) const {
    dto::vacancy::GetDTO dto;

    if (request.HasArg("limit")) {
        dto.limit = std::stoi(request.GetArg("limit"));
    }
    if (request.HasArg("offset")) {
        dto.offset = std::stoi(request.GetArg("offset"));
    }
    if (request.HasArg("query")) {
        dto.query = request.GetArg("query");
    }
    auto res = vacancy_storage_ptr_->GetVacancies(dto);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

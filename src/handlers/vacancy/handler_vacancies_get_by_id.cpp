#include "handler_vacancies_get_by_id.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacanciesGetById::HandlerVacanciesGetById(const ComponentConfig& config,
                                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), vacancy_service_(config, component_context) {
}

Value HandlerVacanciesGetById::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto id = ParseUuidPathArg(request, "id");
    auto res = vacancy_service_.GetVacancyById(id);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

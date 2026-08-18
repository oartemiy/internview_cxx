#include "handler_vacancies_get_by_id.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacanciesGetById::HandlerVacanciesGetById(const ComponentConfig& config,
                                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()) {
}

Value HandlerVacanciesGetById::HandleRequestJsonThrow(
    const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    auto res = vacancy_storage_ptr_->GetVacancyById(id);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

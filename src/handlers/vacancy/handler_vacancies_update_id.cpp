#include "handler_vacancies_update_id.hpp"

#include "dto/vacancy_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyUpdate::HandlerVacancyUpdate(const ComponentConfig& config,
                                           const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerVacancyUpdate::HandleRequestJsonThrow(const HttpRequest& request,
                                                   const Value& request_json,
                                                   [[maybe_unused]] RequestContext& context) const {
    if (request_json.IsEmpty()) {
        throw ClientError(MakeObject("message", "Empty json"));
    }
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }

    auto dto = request_json.As<dto::vacancy::UpdateDTO>();
    dto.id = id;

    auto updated_model = vacancy_storage_ptr_->UpdateVacancy(dto);

    return ValueBuilder(updated_model).ExtractValue();
}

}  // namespace internview::handlers

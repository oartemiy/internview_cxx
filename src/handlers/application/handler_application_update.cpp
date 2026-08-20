#include "handler_application_update.hpp"

#include "components/internview_component.hpp"
#include "dto/application_dto.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationUpdate::HandlerApplicationUpdate(const ComponentConfig& config,
                                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetApplicationStoragePtr()),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()) {
}

Value HandlerApplicationUpdate::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    auto dto = request_json.As<dto::application::UpdateDTO>();
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    dto.id = id;
    if (auth_res.role == "intern") {
        if ((dto.has_cover_letter_in_request_json || dto.has_cv_id_in_request_json) &&
            !dto.has_status_in_request_json) {
            auto res = application_storage_ptr_->UpdateApplication(dto);
            return ValueBuilder(res).ExtractValue();
        } else {
            throw ClientError(MakeObject("message", "invalid intern request_json"));
        }
    } else {
        // TODO: bottle neck. Delete
        if (vacancy_storage_ptr_
                ->GetVacancyById(application_storage_ptr_->GetApplicationById(id).vacancy_id)
                .recruiter_id != auth_res.user_id) {
            throw ClientError(
                MakeObject("message", "This application does not belongs to your vacancy"));
        }
        if (dto.has_status_in_request_json &&
            (!dto.has_cover_letter_in_request_json && !dto.has_cv_id_in_request_json)) {
            auto res = application_storage_ptr_->UpdateApplication(dto);
            return ValueBuilder(res).ExtractValue();
        } else {
            throw ClientError(MakeObject("message", "invalid recruiter request_json"));
        }
    }
}

}  // namespace internview::handlers

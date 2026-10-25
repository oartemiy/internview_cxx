#include "handler_application_update.hpp"

#include "dto/application_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationUpdate::HandlerApplicationUpdate(const ComponentConfig& config,
                                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_service_(config, component_context),
      vacancy_service_(config, component_context),
      cv_service_(config, component_context) {
}

Value HandlerApplicationUpdate::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    auto id = ParseUuidPathArg(request, "id");
    auto dto = ParseJsonBody<dto::application::UpdateDTO>(request_json);
    dto.id = id;
    if (auth_res.role == "intern") {
        if (application_service_.GetApplicationById(id).intern_id != auth_res.user_id) {
            throw Forbidden(MakeObject("message", "This application does not belongs to you"));
        }
        if (!(dto.has_cover_letter_in_request_json || dto.has_cv_id_in_request_json) ||
            dto.has_status_in_request_json) {
            throw ClientError(MakeObject("message", "Invalid intern request_json"));
        }
        if (dto.has_cv_id_in_request_json) {
            // throws ResourceNotFound if cv does not belong to the intern
            cv_service_.GetCvById(dto.cv_id, auth_res.user_id);
        }
        auto res = application_service_.UpdateApplication(dto);
        return ValueBuilder(res).ExtractValue();
    } else {
        // throws ResourceNotFound for unknown application
        if (vacancy_service_.GetRecruiterIdByApplicationId(id) != auth_res.user_id) {
            throw Forbidden(
                MakeObject("message", "This application does not belongs to your vacancy"));
        }
        if (dto.has_status_in_request_json &&
            (!dto.has_cover_letter_in_request_json && !dto.has_cv_id_in_request_json)) {
            auto res = application_service_.UpdateApplication(dto);
            return ValueBuilder(res).ExtractValue();
        } else {
            throw ClientError(MakeObject("message", "Invalid recruiter request_json"));
        }
    }
}

}  // namespace internview::handlers

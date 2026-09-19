#include "handler_application_update.hpp"

#include "dto/application_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationUpdate::HandlerApplicationUpdate(const ComponentConfig& config,
                                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_service_(config, component_context),

      vacancy_service_(config, component_context) {
}

Value HandlerApplicationUpdate::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();
    auto dto = request_json.As<dto::application::UpdateDTO>();
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    dto.id = id;
    if (auth_res.role == "intern") {
        if ((dto.has_cover_letter_in_request_json || dto.has_cv_id_in_request_json) &&
            !dto.has_status_in_request_json) {
            auto res = application_service_.UpdateApplication(dto);
            return ValueBuilder(res).ExtractValue();
        } else {
            throw ClientError(MakeObject("message", "Invalid intern request_json"));
        }
    } else {
        if (vacancy_service_.GetRecruiterIdByApplicationId(id) != auth_res.user_id) {
            throw ClientError(
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

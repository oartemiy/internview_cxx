#include "handler_cv_update.hpp"

#include "dto/cv_dto.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvUpdate::HandlerCvUpdate(const ComponentConfig& config,
                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context), cv_service_(config, component_context) {
}

Value HandlerCvUpdate::HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                              [[maybe_unused]] RequestContext& context) const {
    if (request_json.IsEmpty()) {
        throw ClientError(MakeObject("message", "Empty request data body. Nothing to update"));
    }
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "intern");

    auto dto = ParseJsonBody<dto::cv::UpdateDTO>(request_json);
    if (dto.has_title_in_request) {
        ValidateLength("title", dto.title, 1, 200);
    }
    // NOTE: cv_pdf is a file storage key, it can only be removed here (set to null)
    if (dto.has_cv_pdf_in_request && dto.cv_pdf) {
        throw ClientError(
            MakeObject("message", "cv_pdf can only be set to null, use POST /cvs/{id}/pdf"));
    }
    dto.user_id = auth_res.user_id;
    dto.id = ParseUuidPathArg(request, "id");

    auto resp_dto = cv_service_.UpdateCv(dto);
    return ValueBuilder(resp_dto).ExtractValue();
}

}  // namespace internview::handlers

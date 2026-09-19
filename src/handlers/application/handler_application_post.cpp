#include "handler_application_post.hpp"

#include "dto/application_dto.hpp"
#include "models/cv.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationPost::HandlerApplicationPost(const ComponentConfig& config,
                                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_service_(config, component_context),
      cv_service_(config, component_context) {
}

Value HandlerApplicationPost::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {

    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }

    auto dto = request_json.As<dto::application::CreateDTO>();
    dto.intern_id = auth_res.user_id;
    auto user_cvs = cv_service_.GetUserCvs(auth_res.user_id);
    if (std::ranges::find(user_cvs, dto.cv_id, &models::CV::id) == user_cvs.end()) {
        throw userver::server::handlers::ClientError(userver::formats::json::MakeObject(
            "message", "Cv " + boost::uuids::to_string(dto.cv_id) +
                           " does not belongs to you or does not exists"));
    }
    auto model = application_service_.CreateApplication(dto);

    return ValueBuilder(model).ExtractValue();
}

}  // namespace internview::handlers

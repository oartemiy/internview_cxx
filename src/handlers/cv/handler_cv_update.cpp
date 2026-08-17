#include "handler_cv_update.hpp"

#include "components/internview_component.hpp"
#include "dto/cv_dto.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvUpdate::HandlerCvUpdate(const ComponentConfig& config,
                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()) {
}

Value HandlerCvUpdate::HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                              [[maybe_unused]] RequestContext& context) const {
    if (request_json.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            MakeObject("message", "Empty request data body. Nothing to update"));
    }
    auto auth_path = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_path);
    auto dto = request_json.As<dto::cv::UpdateDTO>();
    auto id = request.GetPathArg("id");
    dto.user_id = auth_res.user_id;
    dto.id = boost::uuids::uuid_from_string(id);

    auto resp_dto = cv_storage_ptr_->UpdateCv(dto);
    return ValueBuilder(resp_dto).ExtractValue();
}

}  // namespace internview::handlers

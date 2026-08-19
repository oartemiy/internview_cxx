#include "handler_application_post.hpp"

#include "components/internview_component.hpp"
#include "dto/application_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerApplicationPost::HandlerApplicationPost(const ComponentConfig& config,
                                               const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetApplicationStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerApplicationPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }

    auto dto = request_json.As<dto::application::CreateDTO>();
    dto.intern_id = auth_res.user_id;

    auto model = application_storage_ptr_->CreateApplication(dto);

    return ValueBuilder(model).ExtractValue();
}

}  // namespace internview::handlers

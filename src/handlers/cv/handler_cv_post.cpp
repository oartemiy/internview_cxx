#include "handler_cv_post.hpp"

namespace internview::handlers {

HandlerCvPost::HandlerCvPost(const ComponentConfig& config,
                             const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()) {
}

Value HandlerCvPost::HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                            [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<internview::dto::cv::CreateDTO>();
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }

    dto.user_id = auth_res.user_id;
    auto res = cv_storage_ptr_->CreateCv(dto);

    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

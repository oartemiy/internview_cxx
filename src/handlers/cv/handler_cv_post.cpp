#include "handler_cv_post.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvPost::HandlerCvPost(const ComponentConfig& config,
                             const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()) {
}

Value HandlerCvPost::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                            const Value& request_json,
                                            [[maybe_unused]] RequestContext& context) const {
    auto dto = request_json.As<internview::dto::cv::CreateDTO>();
    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role != "intern") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }

    dto.user_id = auth_res.user_id;
    auto res = cv_storage_ptr_->CreateCv(dto);

    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

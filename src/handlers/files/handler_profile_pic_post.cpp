#include "handler_profile_pic_post.hpp"

#include "components/internview_component.hpp"
#include "userver/server/http/http_status.hpp"

namespace internview::handlers {

HandlerProfilePicPost::HandlerProfilePicPost(const ComponentConfig& config,
                                             const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

std::string HandlerProfilePicPost::HandleRequestThrow(
    const HttpRequest& request, [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    // LOG_INFO() << auth_header;
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    auto user_id = auth_res.user_id;
    // LOG_INFO() << token;
    const auto& file_arg = request.GetFormDataArg("file");
    // LOG_INFO() << *file_arg.filename;
    if (file_arg.filename->empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::BadRequest);
        return "Empty file";
    }
    user_storage_ptr_->UploadProfilePic(user_id, file_arg);
    return "Uploaded";
}
}  // namespace internview::handlers

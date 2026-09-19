#include "handler_profile_pic_post.hpp"

#include "userver/server/http/http_status.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerProfilePicPost::HandlerProfilePicPost(const ComponentConfig& config,
                                             const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context), user_service_(config, component_context) {
}

std::string HandlerProfilePicPost::HandleRequestThrow(
    const HttpRequest& request, [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();

    auto user_id = auth_res.user_id;
    const auto& file_arg = request.GetFormDataArg("file");
    if (file_arg.filename->empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::BadRequest);
        return "Empty file";
    }
    user_service_.UploadProfilePic(user_id, file_arg);
    return "Uploaded";
}
}  // namespace internview::handlers

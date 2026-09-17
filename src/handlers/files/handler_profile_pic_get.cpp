#include "handler_profile_pic_get.hpp"

#include "components/user_storage_component.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerProfilePicGet::HandlerProfilePicGet(const ComponentConfig& config,
                                           const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      user_storage_ptr_(
          component_context.FindComponent<internview::components::UserStorageComponent>()
              .GetStorage()) {
}

std::string HandlerProfilePicGet::HandleRequestThrow(
    const HttpRequest& request, [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();

    auto user_id = auth_res.user_id;

    auto opt = user_storage_ptr_->GetProfilePic(user_id);
    if (!opt) {
        return "";
    }
    const auto& [pic, file] = *opt;
    if (pic.ends_with(".png")) {
        request.GetHttpResponse().SetContentType("image/png");
    } else if (pic.ends_with(".jpg") || pic.ends_with(".jpeg")) {
        request.GetHttpResponse().SetContentType("image/jpeg");
    }
    request.SetResponseStatus(userver::http::OK);
    return file;
}

}  // namespace internview::handlers

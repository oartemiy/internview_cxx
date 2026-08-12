#include "handler_profile_pic_get.hpp"

#include "components/internview_component.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_base.hpp"

namespace internview::handlers {

HandlerProfilePicGet::HandlerProfilePicGet(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

std::string HandlerProfilePicGet::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    [[maybe_unused]] userver::server::request::RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.size() < 7) {
        request.SetResponseStatus(userver::http::kUnauthorized);
        return "";
    }
    auto token = auth_header.substr(7);
    auto opt = user_storage_ref_.GetProfilePic(token);
    if (!opt) {
        request.SetResponseStatus(userver::v3_2_rc::http::kBadRequest);
        return "";
    }
    const auto& [pic, file] = *opt;
    if (pic.ends_with(".png")) {
        request.GetHttpResponse().SetContentType("image/png");
    } else if (pic.ends_with(".jpg") || pic.ends_with(".jpeg")) {
        request.GetHttpResponse().SetContentType("image/jpeg");
    }
    request.SetResponseStatus(userver::v3_2_rc::http::OK);
    return file;
        
        
}

}  // namespace internview::handlers

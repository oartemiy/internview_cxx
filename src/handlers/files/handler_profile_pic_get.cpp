#include "handler_profile_pic_get.hpp"

#include "components/internview_component.hpp"
#include "errors/errors.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_base.hpp"

namespace internview::handlers {

HandlerProfilePicGet::HandlerProfilePicGet(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerBase(config, component_context),
      user_storage_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetAuthServicePtr()) {
}

std::string HandlerProfilePicGet::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    [[maybe_unused]] userver::server::request::RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->IsAuthorized(auth_header);
    if (!auth_res) {
        request.SetResponseStatus(userver::http::kUnauthorized);
        return "kUnauthorized";
    }
    auto user_id = *auth_res;
    try {
        auto opt = user_storage_ptr_->GetProfilePic(user_id);
        const auto& [pic, file] = *opt;
        if (pic.ends_with(".png")) {
            request.GetHttpResponse().SetContentType("image/png");
        } else if (pic.ends_with(".jpg") || pic.ends_with(".jpeg")) {
            request.GetHttpResponse().SetContentType("image/jpeg");
        }
        request.SetResponseStatus(userver::http::OK);
        return file;
    } catch (errors::NotFoundError& e) {
        request.SetResponseStatus(userver::http::kNotFound);
        return e.what();
    }
}

}  // namespace internview::handlers

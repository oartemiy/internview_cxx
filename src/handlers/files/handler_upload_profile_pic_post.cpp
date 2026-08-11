#include "handler_upload_profile_pic_post.hpp"

#include "components/internview_component.hpp"
#include "userver/logging/log.hpp"
#include "userver/server/http/http_status.hpp"

namespace internview::handlers {

HandlerUploadProfilePicPost::HandlerUploadProfilePicPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

std::string HandlerUploadProfilePicPost::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    [[maybe_unused]] userver::server::request::RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    LOG_INFO() << auth_header;
    if (auth_header.empty() || auth_header.substr(7).empty()) {
        // userver::formats::json::ValueBuilder buidler;
        // buidler["error"] = "unauthorized";
        return "unauthorized";
    }
    auto token = auth_header.substr(7);
    LOG_INFO() << token;
    const auto& file_arg = request.GetFormDataArg("file");
    LOG_INFO() << *file_arg.filename;
    if (file_arg.filename->empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::BadRequest);
        return "empty file";
    }
    auto res = user_storage_ref_.UploadProfilePic(token, file_arg);
    if (res.empty()) {
        // userver::formats::json::ValueBuilder builder;
        // builder["error"] = "something wrong";
        return "something wrong";
    }
    // userver::formats::json::ValueBuilder builder;
    // builder["profile_pic"] = res;
    return res;
}

}  // namespace internview::handlers

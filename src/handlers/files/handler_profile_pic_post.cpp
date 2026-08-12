#include "handler_profile_pic_post.hpp"

#include "components/internview_component.hpp"
#include "storages/user_storage.hpp"
#include "userver/logging/log.hpp"
#include "userver/server/http/http_status.hpp"

namespace internview::handlers {

HandlerProfilePicPost::HandlerProfilePicPost(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerBase(config, component_context),
      user_storage_ref_(
          component_context.FindComponent<internview::components::InternviewComponent>()
              .GetUserStorageRef()) {
}

std::string HandlerProfilePicPost::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    [[maybe_unused]] userver::server::request::RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    // LOG_INFO() << auth_header;
    if (auth_header.empty() || auth_header.substr(7).empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return "Unauthorized";
    }
    auto token = auth_header.substr(7);
    // LOG_INFO() << token;
    const auto& file_arg = request.GetFormDataArg("file");
    // LOG_INFO() << *file_arg.filename;
    if (file_arg.filename->empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::BadRequest);
        return "Empty file";
    }
    auto res = user_storage_ref_.UploadProfilePic(token, file_arg);
    if (!res.has_value()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::BadRequest);
        switch (res.error()) {
            case storages::UserStorage::UploadError::BadJWT:
                return "Expired or bad JWT token";
            case storages::UserStorage::UploadError::InvalidFileType:
                return "Invalid file extansion. Supported formats: jpg, jpeg, png";
            case storages::UserStorage::UploadError::UnknownError:
                return "Error while saving file";
        }
    }
    return *res;
}

}  // namespace internview::handlers

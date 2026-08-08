#include "handler_user_post.hpp"

#include <optional>
#include <string>

#include "components/internview_component.hpp"
#include "dto/user_dto.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

HandlerUserPost::HandlerUserPost(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context),
      user_storage_ref_(
          component_context
              .FindComponent<internview::components::InternviewComponent>("internview-component")
              .GetUserStorageRef()) {
}

HandlerUserPost::Value HandlerUserPost::HandleRequestJsonThrow(
    [[maybe_unused]] const userver::server::http::HttpRequest& request, const Value& request_json,
    [[maybe_unused]] userver::server::request::RequestContext& context) const {
    dto::user::CreateDTO dto;
    dto.login = request_json["login"].As<std::string>();
    dto.password = request_json["password"].As<std::string>();
    dto.name = request_json["name"].As<std::string>();
    dto.description = request_json["description"].As<std::optional<std::string>>(std::nullopt);
    dto.profile_pic = request_json["profile_pic"].As<std::optional<std::string>>(std::nullopt);
    dto.role = request_json["role"].As<std::string>();
    auto res = user_storage_ref_.CreateUser(dto);
    return res.ToJSON();
}
}  // namespace internview::handlers

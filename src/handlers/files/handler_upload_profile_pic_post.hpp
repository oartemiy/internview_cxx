#pragma once

#include <string_view>

#include "storages/user_storage.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/server/handlers/http_handler_base.hpp"

namespace internview::handlers {
// TODO: make db queries to update profile pic
class HandlerUploadProfilePicPost final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-upload-profile-pic-post";

    HandlerUploadProfilePicPost(const userver::components::ComponentConfig& config,
                                const userver::components::ComponentContext& component_context);

    std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                 userver::server::request::RequestContext& context) const override;

private:
    internview::storages::UserStorage& user_storage_ref_;
};

}  // namespace internview::handlers

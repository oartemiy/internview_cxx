#pragma once

#include <string_view>

#include "storages/user_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerProfilePicPost final : public HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-profile-pic-post";

    HandlerProfilePicPost(const ComponentConfig& config, const ComponentContext& component_context);

    std::string HandleRequestThrow(const HttpRequest& request,
                                   RequestContext& context) const override;

private:
    std::shared_ptr<internview::storages::UserStorage> user_storage_ptr_;
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
};

}  // namespace internview::handlers

#pragma once

#include <string_view>

#include "services/user_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerProfilePicGet final : public HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-profile-pic-get";

    HandlerProfilePicGet(const ComponentConfig& config, const ComponentContext& component_context);

    std::string HandleRequestThrow(const HttpRequest& request,
                                   RequestContext& context) const override;

private:
    services::UserService user_service_;
};

}  // namespace internview::handlers

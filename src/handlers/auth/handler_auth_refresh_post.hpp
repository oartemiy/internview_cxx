#pragma once

#include <string_view>

#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerAuthRefreshPost final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-auth-refresh-post";

    HandlerAuthRefreshPost(const ComponentConfig& config,
                           const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::services::AuthService> auth_service_;
};

}  // namespace internview::handlers

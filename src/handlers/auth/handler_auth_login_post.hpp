#pragma once

#include "storages/user_storage.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerAuthLoginPost final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-auth-login-post";

    HandlerAuthLoginPost(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::storages::UserStorage> user_storage_ptr_;
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
};

}  // namespace internview::handlers

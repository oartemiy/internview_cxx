#pragma once

#include <memory>

#include "services/auth_service.hpp"
#include "storages/user_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerUserUpdate final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-user-update";

    HandlerUserUpdate(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::storages::UserStorage> user_storage_ptr_;
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
};

}  // namespace internview::handlers

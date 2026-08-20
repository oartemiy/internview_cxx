#pragma once

#include <memory>
#include <string_view>

#include "services/auth_service.hpp"
#include "storages/application_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerApplicationDelete final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-application-delete";

    HandlerApplicationDelete(const ComponentConfig& config,
                           const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<storages::ApplicationStorage> application_storage_ptr_;
    std::shared_ptr<services::AuthService> auth_service_ptr_;
};

}  // namespace internview::handlers

#pragma once

#include <string_view>

#include "services/application_service.hpp"
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
    services::ApplicationService application_service_;
};

}  // namespace internview::handlers

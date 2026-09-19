#pragma once

#include "services/user_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerUserUpdate final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-user-update";

    HandlerUserUpdate(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    services::UserService user_service_;
};

}  // namespace internview::handlers

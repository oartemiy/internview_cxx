#pragma once

#include <string_view>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include "utils/common_handler.hpp"

namespace internview::handlers::status {
class HandlerStatusGet final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-status-get";
    HandlerStatusGet(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;
};

}  // namespace internview::handlers::status

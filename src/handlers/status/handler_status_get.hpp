#pragma once
#include <string_view>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::handlers::status {
class HandlerStatusGet final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-status-get";
    HandlerStatusGet(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;
};

}  // namespace internview::handlers::status

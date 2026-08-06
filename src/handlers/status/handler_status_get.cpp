#include "handler_status_get.hpp"

#include "userver/formats/json/value_builder.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers::status {
HandlerStatusGet::HandlerStatusGet(const userver::components::ComponentConfig& config,
                                   const userver::components::ComponentContext& component_context)
    : userver::server::handlers::HttpHandlerJsonBase(config, component_context) {
}

HandlerStatusGet::Value HandlerStatusGet::HandleRequestJsonThrow(
    [[maybe_unused]] const HttpRequest& request, [[maybe_unused]] const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    userver::formats::json::ValueBuilder json_builder;
    json_builder["status"] = "ok";
    json_builder["service"] = "InternView API";
    json_builder["version"] = "0.0.1";
    auto json = json_builder.ExtractValue();
    return json;
}
}  // namespace internview::handlers::status

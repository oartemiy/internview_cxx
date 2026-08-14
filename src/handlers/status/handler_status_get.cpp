#include "handler_status_get.hpp"

namespace internview::handlers::status {
HandlerStatusGet::HandlerStatusGet(const ComponentConfig& config,
                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context) {
}

Value HandlerStatusGet::HandleRequestJsonThrow([[maybe_unused]] const HttpRequest& request,
                                               [[maybe_unused]] const Value& request_json,
                                               [[maybe_unused]] RequestContext& context) const {

    return MakeObject("status", "ok", "service", "InternView API", "version", "0.0.1");
}
}  // namespace internview::handlers::status

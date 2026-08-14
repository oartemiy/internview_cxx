#pragma once

#include "components/internview_component.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/server/http/http_request.hpp"
#include "userver/server/request/request_context.hpp"

namespace internview::handlers {

using userver::server::handlers::HttpHandlerBase;
using userver::server::handlers::HttpHandlerJsonBase;

using userver::components::ComponentConfig;
using userver::components::ComponentContext;

using userver::formats::json::MakeObject;
using userver::formats::json::Value;
using userver::formats::json::ValueBuilder;

using internview::components::InternviewComponent;

using userver::server::http::HttpRequest;
using userver::server::request::RequestContext;

}  // namespace internview::handlers

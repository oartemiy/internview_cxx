#pragma once

#include <string_view>

#include "storages/user_storage.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"

namespace internview::handlers {

class HandlerUserDelete final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-user-delete";

    HandlerUserDelete(const userver::components::ComponentConfig& config,
                      const userver::components::ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    const internview::storages::UserStorage& user_storage_;
};

}  // namespace internview::handlers

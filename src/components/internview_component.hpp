#pragma once

#include <string>
#include <string_view>

#include "services/jwt_service.hpp"
#include "storages/user_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"

namespace internview::components {

using userver::components::ComponentConfig;
using userver::components::ComponentContext;

class InternviewComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "internview-component";

    struct SecretKey {
        std::string key;
        SecretKey(const userver::formats::json::Value& doc)
            : key(doc["jwt_secret"].As<std::string>()) {
        }
    };

    InternviewComponent(const ComponentConfig& config, const ComponentContext& component_context);

    const internview::storages::UserStorage& GetUserStorageRef() const {
        return user_storage_;
    }

private:
    internview::storages::UserStorage user_storage_;
    // internview::services::JwtService jwt_service_;
};

}  // namespace internview::components

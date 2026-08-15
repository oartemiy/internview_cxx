#pragma once

#include <memory>
#include <string_view>

#include "services/auth_service.hpp"
#include "storages/cv_storage.hpp"
#include "storages/user_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

using userver::components::ComponentConfig;
using userver::components::ComponentContext;

class InternviewComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "internview-component";

    InternviewComponent(const ComponentConfig& config, const ComponentContext& component_context);

    std::shared_ptr<internview::services::AuthService> GetAuthServicePtr() {
        return auth_service_ptr_;
    }

    std::shared_ptr<internview::storages::UserStorage> GetUserStoragePtr() {
        return user_storage_ptr_;
    }

    std::shared_ptr<internview::storages::CvStorage> GetCvStoragePtr() {
        return cv_storage_ptr_;
    }

private:
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
    std::shared_ptr<internview::storages::UserStorage> user_storage_ptr_;
    std::shared_ptr<internview::storages::CvStorage> cv_storage_ptr_;
};

}  // namespace internview::components

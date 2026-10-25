#pragma once

#include "repositories/interfaces/application_repository.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class ApplicationRepositoryComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "application-repository";

    explicit ApplicationRepositoryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::repositories::interfaces::IApplicationRepository> GetRepository() {
        return application_repository_;
    }

private:
    std::shared_ptr<internview::repositories::interfaces::IApplicationRepository> application_repository_;
};

}  // namespace internview::components

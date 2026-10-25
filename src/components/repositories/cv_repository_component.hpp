#pragma once

#include <string_view>

#include "repositories/interfaces/cv_repository.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class CvRepositoryComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "cv-repository";

    explicit CvRepositoryComponent(const userver::components::ComponentConfig& config,
                                const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::repositories::interfaces::ICvRepository> GetRepository() {
        return cv_repository_;
    }

private:
    std::shared_ptr<internview::repositories::interfaces::ICvRepository> cv_repository_;
};

}  // namespace internview::components

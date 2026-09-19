#pragma once

#include <string_view>

#include "storages/interfaces/cv_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class CvStorageComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "cv-storage";

    explicit CvStorageComponent(const userver::components::ComponentConfig& config,
                                const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::storages::interfaces::ICvStorage> GetStorage() {
        return cv_storage_;
    }

private:
    std::shared_ptr<internview::storages::interfaces::ICvStorage> cv_storage_;
};

}  // namespace internview::components

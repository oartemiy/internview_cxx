#pragma once

#include <string_view>

#include "storages/interfaces/file_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class PdfStorageComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "pdf-storage";

    explicit PdfStorageComponent(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context);

    std::shared_ptr<storages::interfaces::IFileStorage> GetStorage() {
        return storage_;
    }

    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    std::shared_ptr<storages::interfaces::IFileStorage> storage_;
};

}  // namespace internview::components

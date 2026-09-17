#include "img_storage_component.hpp"

#include <memory>
#include <string>
#include <vector>

#include "storages/local/local_file_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/yaml_config/merge_schemas.hpp"
#include "userver/yaml_config/schema.hpp"

namespace internview::components {

ImgStorageComponent::ImgStorageComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context) {
    auto& fs_tp = component_context.GetTaskProcessor(config["fs-task-processor"].As<std::string>());
    auto base_dir = config["base-dir"].As<std::string>();
    std::vector<std::string> extensions = config["supported-extensions"].As<std::vector<std::string>>();
    storage_ = std::make_shared<internview::storages::local::LocalFileStorage>(
        fs_tp, base_dir, std::move(extensions));
}

userver::yaml_config::Schema ImgStorageComponent::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: File storage component for images
additionalProperties: false
properties:
    base-dir:
        type: string
        description: Base directory for storing files
    fs-task-processor:
        type: string
        description: Task processor for file operations
    supported-extensions:
        type: array
        description: List of allowed file extensions
        items:
            type: string
            description: file extension
)");
}

}  // namespace internview::components

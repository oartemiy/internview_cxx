#include "cv_storage_component.hpp"

#include <memory>

#include "storages/postgres/postgres_cv_storage.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

CvStorageComponent::CvStorageComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context) {
    cv_storage_ = std::make_shared<internview::storages::postgres::PostgresCvStorage>(
        config, component_context);
}

}  // namespace internview::components

#include "cv_repository_component.hpp"

#include <memory>

#include "repositories/postgres/postgres_cv_repository.hpp"
#include "userver/components/component_base.hpp"

namespace internview::components {

CvRepositoryComponent::CvRepositoryComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context) {
    cv_repository_ = std::make_shared<internview::repositories::postgres::PostgresCvRepository>(
        config, component_context);
}

}  // namespace internview::components

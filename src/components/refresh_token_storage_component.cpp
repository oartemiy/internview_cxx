#include "refresh_token_storage_component.hpp"

#include <memory>

#include "storages/postgres/postgres_refresh_token_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/logging/log.hpp"
#include "userver/yaml_config/merge_schemas.hpp"
#include "userver/yaml_config/schema.hpp"

namespace internview::components {

RefreshTokenStorage::RefreshTokenStorage(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& component_context)
    : userver::components::ComponentBase(config, component_context),
      refresh_token_storage_(
          std::make_shared<internview::storages::postgres::PostgresRefreshTokenStorage>(
              config, component_context)),
      periodic_task_("clear-expired-tokens",
                     userver::utils::PeriodicTask::Settings(std::chrono::months(1)), [this] {
                         LOG_INFO() << "Stared to cleanup expired tokens";
                         refresh_token_storage_->ClearExpiredTokens();
                         LOG_INFO() << "Finished to cleanup expired tokens";
                     }) {
}

userver::yaml_config::Schema RefreshTokenStorage::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: refresh token storage
additionalProperties: false
properties:
    token-lifetime-seconds:
        type: integer
        description: Refresh token lifetime in seconds
)");
}

}  // namespace internview::components

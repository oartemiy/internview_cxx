#pragma once

#include <string_view>

#include "storages/interfaces/vacancy_storage.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class VacancyStorageComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "vacancy-storage-component";

    explicit VacancyStorageComponent(
        const userver::components::ComponentConfig& confing,
        const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::storages::interfaces::IVacancyStorage> GetStorage() {
        return vacancy_storage_;
    }

private:
    std::shared_ptr<internview::storages::interfaces::IVacancyStorage> vacancy_storage_;
};

}  // namespace internview::components

#pragma once

#include <string_view>

#include "repositories/interfaces/vacancy_repository.hpp"
#include "userver/components/component_base.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::components {

class VacancyRepositoryComponent final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "vacancy-repository";

    explicit VacancyRepositoryComponent(
        const userver::components::ComponentConfig& confing,
        const userver::components::ComponentContext& component_context);

    std::shared_ptr<internview::repositories::interfaces::IVacancyRepository> GetRepository() {
        return vacancy_repository_;
    }

private:
    std::shared_ptr<internview::repositories::interfaces::IVacancyRepository> vacancy_repository_;
};

}  // namespace internview::components

#pragma once

#include <memory>
#include <string_view>

#include "services/auth_service.hpp"
#include "storages/vacancy_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerVacanciesGetMe final : public HttpHandlerJsonBase {
public:
    HandlerVacanciesGetMe(const ComponentConfig& config, const ComponentContext& component_context);

    static constexpr std::string_view kName = "handler-vacancies-get-me";

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<services::AuthService> auth_service_ptr_;
    std::shared_ptr<storages::VacancyStorage> vacancy_storage_ptr_;
};

}  // namespace internview::handlers

#pragma once

#include <memory>
#include <string_view>

#include "services/auth_service.hpp"
#include "services/vacancy_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerVacanciesGet final : public HttpHandlerJsonBase {
public:
    HandlerVacanciesGet(const ComponentConfig& config, const ComponentContext& component_context);

    static constexpr std::string_view kName = "handler-vacancies-get";

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<services::AuthService> auth_service_ptr_;
    services::VacancyService vacancy_service_;
};

}  // namespace internview::handlers

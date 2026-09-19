#pragma once

#include "services/application_service.hpp"
#include "services/vacancy_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerVacancyApplicationsGet final : public HttpHandlerJsonBase {
public:
    HandlerVacancyApplicationsGet(const ComponentConfig& config,
                                  const ComponentContext& component_context);

    static constexpr std::string_view kName = "handler-vacancy-applications-get";

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    services::VacancyService vacancy_service_;
    services::ApplicationService application_service_;
};

}  // namespace internview::handlers

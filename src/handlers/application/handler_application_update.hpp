#pragma once

#include <string_view>

#include "services/application_service.hpp"
#include "services/vacancy_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerApplicationUpdate final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-application-update";

    HandlerApplicationUpdate(const ComponentConfig& config,
                             const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    services::ApplicationService application_service_;

    services::VacancyService vacancy_service_;
};

}  // namespace internview::handlers

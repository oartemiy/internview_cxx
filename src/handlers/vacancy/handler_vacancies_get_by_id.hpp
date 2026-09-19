#pragma once

#include <string_view>

#include "services/vacancy_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerVacanciesGetById final : public HttpHandlerJsonBase {
public:
    HandlerVacanciesGetById(const ComponentConfig& config,
                            const ComponentContext& component_context);

    static constexpr std::string_view kName = "handler-vacancies-get-by-id";

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    services::VacancyService vacancy_service_;
};

}  // namespace internview::handlers

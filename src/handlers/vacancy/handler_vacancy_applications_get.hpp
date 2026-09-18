#pragma once

#include <memory>

#include "storages/interfaces/application_storage.hpp"
#include "storages/vacancy_storage.hpp"
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
    std::shared_ptr<internview::storages::VacancyStorage> vacancy_storage_ptr_;
    std::shared_ptr<internview::storages::interfaces::IApplicationStorage> application_storage_ptr_;
};

}  // namespace internview::handlers

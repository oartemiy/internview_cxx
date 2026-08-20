#pragma once

#include <memory>

#include "services/auth_service.hpp"
#include "storages/application_storage.hpp"
#include "storages/vacancy_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerVacancyApplicationsGet final : public HttpHandlerJsonBase {
public:
    HandlerVacancyApplicationsGet(const ComponentConfig& config, const ComponentContext& component_context);

    static constexpr std::string_view kName = "handler-vacancy-applications-get";

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::storages::VacancyStorage> vacancy_storage_ptr_;
    std::shared_ptr<internview::storages::ApplicationStorage> application_storage_ptr_;
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
};

}  // namespace internview::handlers

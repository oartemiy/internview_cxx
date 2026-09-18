#pragma once

#include <memory>
#include <string_view>

#include "storages/interfaces/application_storage.hpp"
#include "storages/interfaces/vacancy_storage.hpp"
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
    std::shared_ptr<internview::storages::interfaces::IApplicationStorage> application_storage_ptr_;

    std::shared_ptr<storages::interfaces::IVacancyStorage> vacancy_storage_ptr_;
};

}  // namespace internview::handlers

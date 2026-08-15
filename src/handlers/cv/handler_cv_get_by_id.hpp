#pragma once

#include <memory>
#include "services/auth_service.hpp"
#include "storages/cv_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvGetById final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-cv-get-by-id";

    HandlerCvGetById(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
    std::shared_ptr<internview::storages::CvStorage> cv_storage_ptr_;
};

}  // namespace internview::handlers

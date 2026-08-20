#pragma once

#include <memory>
#include <string_view>

#include "services/auth_service.hpp"
#include "storages/application_storage.hpp"
#include "storages/cv_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvPdfGet final : public HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-cv-pdf-get";

    HandlerCvPdfGet(const ComponentConfig& config, const ComponentContext& component_context);

    std::string HandleRequestThrow(const HttpRequest& request, RequestContext& context) const;

private:
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
    std::shared_ptr<internview::storages::ApplicationStorage> application_storage_ptr_;
    std::shared_ptr<internview::storages::CvStorage> cv_storage_ptr_;
};

}  // namespace internview::handlers

#pragma once

#include <string_view>
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvPdfPost final : public HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-cv-pdf-post";

    HandlerCvPdfPost(const ComponentConfig& config, const ComponentContext& component_context);

    std::string HandleRequest(HttpRequest& request, RequestContext& context) const override;

private:
    std::shared_ptr<internview::services::AuthService> auth_service_ptr_;
    std::shared_ptr<internview::storages::CvStorage> cv_storage_ptr_;
};

}  // namespace internview::handlers

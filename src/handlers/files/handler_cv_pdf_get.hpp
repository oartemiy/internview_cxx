#pragma once

#include <string_view>

#include "services/application_service.hpp"
#include "services/cv_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvPdfGet final : public HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-cv-pdf-get";

    HandlerCvPdfGet(const ComponentConfig& config, const ComponentContext& component_context);

    std::string HandleRequestThrow(const HttpRequest& request, RequestContext& context) const;

private:
    services::ApplicationService application_service_;
    services::CvService cv_service_;
};

}  // namespace internview::handlers

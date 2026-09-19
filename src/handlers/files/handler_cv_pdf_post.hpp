#pragma once

#include <string_view>

#include "services/cv_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvPdfPost final : public HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-cv-pdf-post";

    HandlerCvPdfPost(const ComponentConfig& config, const ComponentContext& component_context);

    std::string HandleRequest(HttpRequest& request, RequestContext& context) const override;

private:
    services::CvService cv_service_;
};

}  // namespace internview::handlers

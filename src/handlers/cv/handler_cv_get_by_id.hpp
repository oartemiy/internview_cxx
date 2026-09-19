#pragma once

#include "services/application_service.hpp"
#include "services/cv_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvGetById final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-cv-get-by-id";

    HandlerCvGetById(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    services::ApplicationService application_service_;
    services::CvService cv_service_;
};

}  // namespace internview::handlers

#pragma once

#include <string_view>

#include "services/cv_service.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvDelete final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-cv-delete";

    HandlerCvDelete(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    services::CvService cv_service_;
};

}  // namespace internview::handlers

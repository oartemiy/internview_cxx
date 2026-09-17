#pragma once

#include <memory>

#include "storages/application_storage.hpp"
#include "storages/interfaces/cv_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerCvGetById final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-cv-get-by-id";

    HandlerCvGetById(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::storages::ApplicationStorage> application_storage_;
    std::shared_ptr<internview::storages::interfaces::ICvStorage> cv_storage_ptr_;
};

}  // namespace internview::handlers

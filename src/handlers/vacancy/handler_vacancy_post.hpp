#pragma once

#include <memory>

#include "storages/interfaces/vacancy_storage.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerVacancyPost final : public HttpHandlerJsonBase {
public:
    HandlerVacancyPost(const ComponentConfig& config, const ComponentContext& component_context);

    static constexpr std::string_view kName = "handler-vacancy-post";

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::storages::interfaces::IVacancyStorage> vacancy_storage_ptr_;
};

}  // namespace internview::handlers

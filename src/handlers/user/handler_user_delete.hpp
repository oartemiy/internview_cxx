#pragma once

#include <memory>
#include <string_view>

#include "storages/interfaces/cv_storage.hpp"
#include "storages/interfaces/user_storage.hpp"
#include "storages/interfaces/vacancy_storage.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

class HandlerUserDelete final : public HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-user-delete";

    HandlerUserDelete(const ComponentConfig& config, const ComponentContext& component_context);

    Value HandleRequestJsonThrow(const HttpRequest& request, const Value& request_json,
                                 RequestContext& context) const override;

private:
    std::shared_ptr<internview::storages::interfaces::IUserStorage> user_storage_ptr_;
    std::shared_ptr<internview::storages::interfaces::ICvStorage> cv_storage_ptr_;
    std::shared_ptr<internview::storages::interfaces::IVacancyStorage> vacancy_storage_ptr_;
};

}  // namespace internview::handlers

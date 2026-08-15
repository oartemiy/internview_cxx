#include "handler_cv_get_by_id.hpp"

#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvGetById::HandlerCvGetById(const ComponentConfig& config,
                                   const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()) {
}

Value HandlerCvGetById::HandleRequestJsonThrow(const HttpRequest& request,
                                               [[maybe_unused]] const Value& request_json,
                                               [[maybe_unused]] RequestContext& context) const {
    auto id = request.GetPathArg("id");
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    auto resp_model =
        cv_storage_ptr_->GetCvById(boost::uuids::uuid_from_string(id), auth_res.user_id);
    return ValueBuilder(resp_model).ExtractValue();
}

}  // namespace internview::handlers

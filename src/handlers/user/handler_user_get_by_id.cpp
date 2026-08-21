#include "handler_user_get_by_id.hpp"

#include "components/internview_component.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerUserGetById::HandlerUserGetById(const ComponentConfig& config,
                                       const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      application_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetApplicationStoragePtr()),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerUserGetById::HandleRequestJsonThrow(const HttpRequest& request,
                                                 [[maybe_unused]] const Value& request_json,
                                                 [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    if (auth_res.user_id == id || auth_res.role == "recruiter") {
        if (auth_res.user_id != id &&
            !application_storage_ptr_->CheckInternApplied(id, auth_res.user_id)) {
            throw ClientError(MakeObject("message", "You do not have roots to do this action"));
        }
        auto res = user_storage_ptr_->GetUserById(id).ToResponseDTO();
        return ValueBuilder(res).ExtractValue();
    } else {
        throw ClientError(MakeObject("message", "invalid role or id for this action"));
    }
}

}  // namespace internview::handlers

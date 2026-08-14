#include "handler_auth_register_post.hpp"

#include "components/internview_component.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerAuthRegisterPost::HandlerAuthRegisterPost(const ComponentConfig& config,
                                                 const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      user_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetUserStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerAuthRegisterPost::HandleRequestJsonThrow(
    const HttpRequest& request, const Value& request_json,
    [[maybe_unused]] RequestContext& context) const {
    dto::user::CreateDTO dto = request_json.As<dto::user::CreateDTO>();
    if (dto.login == "me") {
        request.GetHttpResponse().SetStatus(userver::http::kBadRequest);
        return MakeObject("error", "login: me can not be taken");
    }
    if (dto.password.length() <= 1) {
        request.GetHttpResponse().SetStatus(userver::http::kBadRequest);
        return MakeObject("error", "password must be at least 2 chars");
    }
    auto res = user_storage_ptr_->CreateUser(dto);
    return ValueBuilder(res).ExtractValue();
}

}  // namespace internview::handlers

#include "auth_checker.hpp"

#include <memory>

#include "components/internview_component.hpp"
#include "services/auth_service.hpp"
#include "userver/http/common_headers.hpp"
#include "userver/server/handlers/auth/auth_checker_base.hpp"

namespace internview::auth {

JwtAuthChecker::JwtAuthChecker(std::shared_ptr<services::AuthService> auth_service_ptr)
    : userver::server::handlers::auth::AuthCheckerBase{},
      auth_service_(std::move(auth_service_ptr)) {
}

AuthCheckResult JwtAuthChecker::CheckAuth(const userver::server::http::HttpRequest& request,
                                          userver::server::request::RequestContext& context) const {
    const auto& auth_header = request.GetHeader(userver::http::headers::kAuthorization);
    auto auth_res = auth_service_->CheckAuthorization(auth_header);

    context.SetUserData(auth_res);

    return AuthCheckResult{};
}

JwtAuthCheckerFactory::JwtAuthCheckerFactory(
    const userver::components::ComponentContext& component_context)
    : internview_component_(component_context.FindComponent<components::InternviewComponent>()) {
}

AuthCheckerBasePtr JwtAuthCheckerFactory::MakeAuthChecker(
    [[maybe_unused]] const HandlerAuthConfig& config) const {
    return std::make_shared<JwtAuthChecker>(internview_component_.GetAuthServicePtr());
}

}  // namespace internview::auth

#pragma once

#include <memory>

#include "components/internview_component.hpp"
#include "services/auth_service.hpp"
#include "userver/components/component_context.hpp"
#include "userver/server/handlers/auth/auth_checker_base.hpp"
#include "userver/server/handlers/auth/auth_checker_factory.hpp"
#include "userver/server/handlers/auth/handler_auth_config.hpp"

namespace internview::auth {

using userver::server::handlers::auth::AuthCheckerBasePtr;
using userver::server::handlers::auth::AuthCheckResult;
using userver::server::handlers::auth::HandlerAuthConfig;

class JwtAuthChecker final : public userver::server::handlers::auth::AuthCheckerBase {
public:
    explicit JwtAuthChecker(std::shared_ptr<services::AuthService> auth_service_ptr);

    [[nodiscard]] AuthCheckResult CheckAuth(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

    [[nodiscard]] bool SupportsUserAuth() const noexcept override {
        return true;
    }

private:
    std::shared_ptr<internview::services::AuthService> auth_service_;
};

class JwtAuthCheckerFactory final : public userver::server::handlers::auth::AuthCheckerFactoryBase {
public:
    static constexpr std::string_view kAuthType = "jwt";

    explicit JwtAuthCheckerFactory(const userver::components::ComponentContext& component_context);

    AuthCheckerBasePtr MakeAuthChecker(const HandlerAuthConfig& config) const override;

private:
    internview::components::InternviewComponent& internview_component_;
};

}  // namespace internview::auth

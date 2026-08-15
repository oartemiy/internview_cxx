#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component_list.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/congestion_control/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "components/internview_component.hpp"
#include "handlers/auth/handler_auth_change_password_post.hpp"
#include "handlers/auth/handler_auth_login_post.hpp"
#include "handlers/auth/handler_auth_register_post.hpp"
#include "handlers/cv/handler_cv_get.hpp"
#include "handlers/cv/handler_cv_get_by_id.hpp"
#include "handlers/cv/handler_cv_post.hpp"
#include "handlers/files/handler_profile_pic_get.hpp"
#include "handlers/files/handler_profile_pic_post.hpp"
#include "handlers/status/handler_status_get.hpp"
#include "handlers/user/handler_user_delete.hpp"
#include "handlers/user/handler_user_get.hpp"
#include "handlers/user/handler_user_update.hpp"
#include "userver/storages/secdist/provider_component.hpp"

int main(int argc, char* argv[]) {
    auto component_list =
        userver::components::MinimalServerComponentList()
            .Append<userver::server::handlers::Ping>()
            .Append<userver::components::TestsuiteSupport>()
            .AppendComponentList(userver::clients::http::ComponentList())
            .Append<userver::clients::dns::Component>()
            .Append<userver::server::handlers::TestsControl>()
            .Append<userver::congestion_control::Component>()
            .Append<userver::components::Postgres>("postgres-db")

            .Append<userver::components::DefaultSecdistProvider>("default-secdist-provider")

            .Append<internview::components::InternviewComponent>("internview-component")

            .Append<internview::handlers::status::HandlerStatusGet>()

            .Append<internview::handlers::HandlerAuthRegisterPost>()
            .Append<internview::handlers::HandlerAuthLoginPost>()
            .Append<internview::handlers::HandlerAuthChangePasswordPost>()

            .Append<internview::handlers::HandlerUserDelete>()
            .Append<internview::handlers::HandlerUserUpdate>()
            .Append<internview::handlers::HandlerUserGet>()
            .Append<internview::handlers::HandlerProfilePicPost>()
            .Append<internview::handlers::HandlerProfilePicGet>()

            .Append<internview::handlers::HandlerCvPost>()
            .Append<internview::handlers::HandlerCvGet>()
            .Append<internview::handlers::HandlerCvGetById>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}

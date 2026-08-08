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
#include "handlers/status/handler_status_get.hpp"
#include "handlers/user/handler_user_post.hpp"
#include "userver/storages/secdist/provider_component.hpp"

// TODO: Make internview::utils::password functions async

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
            .Append<internview::handlers::HandlerUserPost>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}

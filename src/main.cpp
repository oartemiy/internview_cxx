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

#include "auth/auth_checker.hpp"
#include "components/img_storage_component.hpp"
#include "components/internview_component.hpp"
#include "components/pdf_storage_component.hpp"
#include "components/user_storage_component.hpp"
#include "handlers/application/handler_application_delete.hpp"
#include "handlers/application/handler_application_post.hpp"
#include "handlers/application/handler_application_update.hpp"
#include "handlers/application/handler_applications_get.hpp"
#include "handlers/auth/handler_auth_change_password_post.hpp"
#include "handlers/auth/handler_auth_login_post.hpp"
#include "handlers/auth/handler_auth_logout_post.hpp"
#include "handlers/auth/handler_auth_refresh_post.hpp"
#include "handlers/auth/handler_auth_register_post.hpp"
#include "handlers/cv/handler_cv_delete.hpp"
#include "handlers/cv/handler_cv_get.hpp"
#include "handlers/cv/handler_cv_get_by_id.hpp"
#include "handlers/cv/handler_cv_post.hpp"
#include "handlers/cv/handler_cv_update.hpp"
#include "handlers/files/handler_cv_pdf_get.hpp"
#include "handlers/files/handler_cv_pdf_post.hpp"
#include "handlers/files/handler_profile_pic_get.hpp"
#include "handlers/files/handler_profile_pic_get_for_recruiter.hpp"
#include "handlers/files/handler_profile_pic_post.hpp"
#include "handlers/status/handler_status_get.hpp"
#include "handlers/user/handler_user_delete.hpp"
#include "handlers/user/handler_user_get.hpp"
#include "handlers/user/handler_user_get_by_id.hpp"
#include "handlers/user/handler_user_update.hpp"
#include "handlers/vacancy/handler_vacancies_delete.hpp"
#include "handlers/vacancy/handler_vacancies_get.hpp"
#include "handlers/vacancy/handler_vacancies_get_by_id.hpp"
#include "handlers/vacancy/handler_vacancies_get_me.hpp"
#include "handlers/vacancy/handler_vacancies_toggle.hpp"
#include "handlers/vacancy/handler_vacancies_update_id.hpp"
#include "handlers/vacancy/handler_vacancy_applications_get.hpp"
#include "handlers/vacancy/handler_vacancy_post.hpp"
#include "userver/server/handlers/auth/auth_checker_factory.hpp"
#include "userver/storages/secdist/provider_component.hpp"

// ?: Add pool for slave HOSTTYPE
// TODO: add DI container for storages
// TODO: add DI container for files storage

int main(int argc, char* argv[]) {
    userver::server::handlers::auth::RegisterAuthCheckerFactory<
        internview::auth::JwtAuthCheckerFactory>();

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

            .Append<internview::components::ImgStorageComponent>("img-storage")
            .Append<internview::components::PdfStorageComponent>("pdf-storage")

            .Append<internview::components::UserStorageComponent>("user-storage")

            .Append<internview::components::InternviewComponent>("internview-component")


            .Append<internview::handlers::status::HandlerStatusGet>()

            .Append<internview::handlers::HandlerAuthRegisterPost>()
            .Append<internview::handlers::HandlerAuthLoginPost>()
            .Append<internview::handlers::HandlerAuthChangePasswordPost>()
            .Append<internview::handlers::HandlerAuthRefreshPost>()
            .Append<internview::handlers::HandlerAuthLogoutPost>()

            .Append<internview::handlers::HandlerUserDelete>()
            .Append<internview::handlers::HandlerUserUpdate>()
            .Append<internview::handlers::HandlerUserGet>()
            .Append<internview::handlers::HandlerProfilePicPost>()
            .Append<internview::handlers::HandlerProfilePicGet>()
            .Append<internview::handlers::HandlerProfilePicGetForRecruiter>()
            .Append<internview::handlers::HandlerUserGetById>()

            .Append<internview::handlers::HandlerCvPost>()
            .Append<internview::handlers::HandlerCvGet>()
            .Append<internview::handlers::HandlerCvGetById>()
            .Append<internview::handlers::HandlerCvUpdate>()
            .Append<internview::handlers::HandlerCvDelete>()
            .Append<internview::handlers::HandlerCvPdfGet>()
            .Append<internview::handlers::HandlerCvPdfPost>()

            .Append<internview::handlers::HandlerVacancyPost>()
            .Append<internview::handlers::HandlerVacanciesGet>()
            .Append<internview::handlers::HandlerVacanciesGetMe>()
            .Append<internview::handlers::HandlerVacanciesGetById>()
            .Append<internview::handlers::HandlerVacancyUpdate>()
            .Append<internview::handlers::HandlerVacanciesDelete>()
            .Append<internview::handlers::HandlerVacancyToggle>()
            .Append<internview::handlers::HandlerVacancyApplicationsGet>()

            .Append<internview::handlers::HandlerApplicationPost>()
            .Append<internview::handlers::HandlerApplicationsGet>()
            .Append<internview::handlers::HandlerApplicationUpdate>()
            .Append<internview::handlers::HandlerApplicationDelete>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}

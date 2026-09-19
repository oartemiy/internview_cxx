#include "handler_cv_pdf_get.hpp"

#include "userver/server/handlers/http_handler_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvPdfGet::HandlerCvPdfGet(const ComponentConfig& config,
                                 const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      application_service_(config, component_context),
      cv_service_(config, component_context) {
}

std::string HandlerCvPdfGet::HandleRequestThrow(const HttpRequest& request,
                                                [[maybe_unused]] RequestContext& context) const {
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    auto auth_res = context.GetUserData<AuthResult>();
    auto intern_id = auth_res.user_id;

    if (auth_res.role == "recruiter") {
        intern_id = application_service_.GetInternIdByCv(id, auth_res.user_id);
    }
    auto opt = cv_service_.GetCvPdf(id, intern_id);

    if (!opt) {
        return "";
    }

    const auto& [pic, file] = *opt;
    request.SetResponseStatus(userver::http::OK);
    request.GetHttpResponse().SetContentType("application/pdf");

    return file;
}

}  // namespace internview::handlers

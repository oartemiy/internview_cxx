#include "handler_cv_pdf_get.hpp"

#include "components/internview_component.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvPdfGet::HandlerCvPdfGet(const ComponentConfig& config,
                                 const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()) {
}

std::string HandlerCvPdfGet::HandleRequestThrow(const HttpRequest& request,
                                                [[maybe_unused]] RequestContext& context) const {
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);

    auto opt = cv_storage_ptr_->GetCvPdf(id, auth_res.user_id);

    if (!opt) {
        return "";
    }

    const auto& [pic, file] = *opt;
    request.SetResponseStatus(userver::http::OK);
    request.GetHttpResponse().SetContentType("application/pdf");

    return file;
}

}  // namespace internview::handlers

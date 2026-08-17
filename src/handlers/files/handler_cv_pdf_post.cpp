#include "handler_cv_pdf_post.hpp"

#include "components/internview_component.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvPdfPost::HandlerCvPdfPost(const ComponentConfig& config,
                                   const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      auth_service_ptr_(component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()),
      cv_storage_ptr_(component_context.FindComponent<InternviewComponent>().GetCvStoragePtr()) {
}

std::string HandlerCvPdfPost::HandleRequest(HttpRequest& request,
                                            [[maybe_unused]] RequestContext& context) const {
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    const auto& file_data = request.GetFormDataArg("file");
    if (file_data.filename->empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::BadRequest);
        return "Empty file";
    }
    cv_storage_ptr_->UploadCvPdf(id, auth_res.user_id, file_data);
    return "Uploaded";
}

}  // namespace internview::handlers

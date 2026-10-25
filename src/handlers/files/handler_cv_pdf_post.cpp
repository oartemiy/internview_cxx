#include "handler_cv_pdf_post.hpp"

#include "userver/server/handlers/http_handler_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerCvPdfPost::HandlerCvPdfPost(const ComponentConfig& config,
                                   const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context), cv_service_(config, component_context) {
}

std::string HandlerCvPdfPost::HandleRequest(HttpRequest& request,
                                            [[maybe_unused]] RequestContext& context) const {
    auto id = ParseUuidPathArg(request, "id");
    auto auth_res = context.GetUserData<AuthResult>();
    RequireRole(auth_res, "intern");

    const auto& file_data = request.GetFormDataArg("file");
    if (!file_data.filename || file_data.filename->empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::BadRequest);
        return "Empty file";
    }
    cv_service_.UploadCvPdf(id, auth_res.user_id, file_data);
    return "Uploaded";
}

}  // namespace internview::handlers

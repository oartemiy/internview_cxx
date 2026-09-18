#include "handler_profile_pic_get_for_recruiter.hpp"

#include "components/application_storage_component.hpp"
#include "components/user_storage_component.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerProfilePicGetForRecruiter::HandlerProfilePicGetForRecruiter(
    const ComponentConfig& config, const ComponentContext& component_context)
    : HttpHandlerBase(config, component_context),
      user_storage_ptr_(
          component_context.FindComponent<internview::components::UserStorageComponent>()
              .GetStorage()),
      application_storage_ptr_(
          component_context.FindComponent<components::ApplicationStorageComponent>().GetStorage()) {
}

std::string HandlerProfilePicGetForRecruiter::HandleRequestThrow(
    const HttpRequest& request, [[maybe_unused]] RequestContext& context) const {
    auto auth_res = context.GetUserData<AuthResult>();

    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "Invalid role for this action"));
    }
    auto id = boost::uuids::uuid_from_string(request.GetPathArg("id"));
    if (application_storage_ptr_->CheckInternApplied(id, auth_res.user_id)) {
        auto opt = user_storage_ptr_->GetProfilePic(id);
        if (!opt) {
            return "";
        }
        const auto& [pic, file] = *opt;
        if (pic.ends_with(".png")) {
            request.GetHttpResponse().SetContentType("image/png");
        } else if (pic.ends_with(".jpg") || pic.ends_with(".jpeg")) {
            request.GetHttpResponse().SetContentType("image/jpeg");
        }
        request.SetResponseStatus(userver::http::OK);
        return file;
    } else {
        throw ClientError(MakeObject("message", "This user does not applied for your vacancies"));
    }
}

}  // namespace internview::handlers

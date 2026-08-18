#include "handler_vacancy_post.hpp"

#include "components/internview_component.hpp"
#include "dto/vacancy_dto.hpp"
#include "utils/common_handler.hpp"

namespace internview::handlers {

HandlerVacancyPost::HandlerVacancyPost(const ComponentConfig& config,
                                       const ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context),
      vacancy_storage_ptr_(
          component_context.FindComponent<InternviewComponent>().GetVacancyStoragePtr()),
      auth_service_ptr_(
          component_context.FindComponent<InternviewComponent>().GetAuthServicePtr()) {
}

Value HandlerVacancyPost::HandleRequestJsonThrow(const HttpRequest& request,
                                                 const Value& request_json,
                                                 [[maybe_unused]] RequestContext& context) const {
    auto auth_header = request.GetHeader("Authorization");
    auto auth_res = auth_service_ptr_->CheckAuthorization(auth_header);
    if (auth_res.role != "recruiter") {
        throw ClientError(MakeObject("message", "invalid role for this action"));
    }
    auto dto = request_json.As<dto::vacancy::CreateDTO>();
    dto.recruiter_id = auth_res.user_id;

    auto model = vacancy_storage_ptr_->CreateVacancy(dto);

    return ValueBuilder(model).ExtractValue();
}

}  // namespace internview::handlers

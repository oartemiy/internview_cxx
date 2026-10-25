#pragma once

#include "userver/server/handlers/exceptions.hpp"

namespace internview::utils {

// userver has no named class for HTTP 403
using Forbidden = userver::server::handlers::ExceptionWithCode<
    userver::server::handlers::HandlerErrorCode::kForbidden>;

}  // namespace internview::utils

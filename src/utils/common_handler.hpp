#pragma once

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <charconv>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "services/auth_service.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/exception.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/server/http/http_request.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/server/request/request_context.hpp"
#include "userver/utils/text_light.hpp"
#include "utils/exceptions.hpp"

namespace internview::handlers {

using userver::server::handlers::HttpHandlerBase;
using userver::server::handlers::HttpHandlerJsonBase;

using userver::components::ComponentConfig;
using userver::components::ComponentContext;

using userver::formats::json::MakeObject;
using userver::formats::json::Value;
using userver::formats::json::ValueBuilder;

using userver::server::http::HttpRequest;
using userver::server::http::HttpStatus;
using userver::server::request::RequestContext;

using userver::server::handlers::ClientError;
using userver::server::handlers::ConflictError;
using userver::server::handlers::ResourceNotFound;
using internview::utils::Forbidden;

using AuthResult = internview::services::AuthService::AuthResult;

/**
 * @brief Parses path argument as uuid
 *
 * @throw ClientError if the argument is not a valid uuid
 */
inline boost::uuids::uuid ParseUuidPathArg(const HttpRequest& request, const std::string& name) {
    const auto& arg = request.GetPathArg(name);
    try {
        return boost::uuids::string_generator{}(arg);
    } catch (const std::exception&) {
        throw ClientError(MakeObject("message", "Invalid uuid in path: " + name));
    }
}

/**
 * @brief Parses request json into dto
 *
 * @throw ClientError on missing fields or wrong types
 */
template <typename T>
T ParseJsonBody(const Value& request_json) {
    try {
        return request_json.As<T>();
    } catch (const userver::formats::json::Exception& e) {
        throw ClientError(MakeObject("message", std::string{"Invalid request body: "} + e.what()));
    }
}

/**
 * @brief Parses optional integer query argument
 *
 * @throw ClientError if the argument is not an integer in [min, max]
 */
inline std::optional<int> ParseIntArg(const HttpRequest& request, const std::string& name, int min,
                                      int max) {
    if (!request.HasArg(name)) {
        return std::nullopt;
    }
    const auto& arg = request.GetArg(name);
    int value = 0;
    const auto [ptr, ec] = std::from_chars(arg.data(), arg.data() + arg.size(), value);
    if (ec != std::errc{} || ptr != arg.data() + arg.size() || value < min || value > max) {
        throw ClientError(MakeObject("message", name + " must be an integer in [" +
                                                    std::to_string(min) + ", " +
                                                    std::to_string(max) + "]"));
    }
    return value;
}

/**
 * @throw Forbidden if user has another role
 */
inline void RequireRole(const AuthResult& auth_res, std::string_view role) {
    if (auth_res.role != role) {
        throw Forbidden(MakeObject("message", "Invalid role for this action"));
    }
}

/**
 * @brief Checks length of utf-8 string in code points
 *
 * @throw ClientError
 */
inline void ValidateLength(std::string_view field, std::string_view value, std::size_t min,
                           std::size_t max) {
    std::size_t length = 0;
    try {
        length = userver::utils::text::utf8::GetCodePointsCount(value);
    } catch (const std::runtime_error&) {
        throw ClientError(MakeObject("message", std::string{field} + " must be valid utf-8"));
    }
    if (length < min || length > max) {
        throw ClientError(MakeObject("message", std::string{field} + " length must be in [" +
                                                    std::to_string(min) + ", " +
                                                    std::to_string(max) + "]"));
    }
}

/**
 * @throw ClientError if value is set and not one of allowed
 */
inline void ValidateEnum(std::string_view field, const std::optional<std::string>& value,
                         const std::vector<std::string_view>& allowed) {
    if (!value) {
        return;
    }
    for (const auto& item : allowed) {
        if (*value == item) {
            return;
        }
    }
    std::string allowed_str;
    for (const auto& item : allowed) {
        allowed_str += (allowed_str.empty() ? "" : ", ") + std::string{item};
    }
    throw ClientError(
        MakeObject("message", std::string{field} + " must be one of: " + allowed_str));
}

}  // namespace internview::handlers

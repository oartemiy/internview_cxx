#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>

#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/parse/common_containers.hpp"
#include "userver/formats/serialize/to.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/formats/serialize/boost_uuid.hpp"

namespace internview::dto::cv {

struct CreateDTO {
    boost::uuids::uuid user_id;
    std::string title;
    std::optional<std::string> description;
    std::optional<std::string> cv_pdf;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<CreateDTO>) {
    CreateDTO dto;
    dto.title = json["title"].As<std::string>();
    dto.description = json["description"].As<std::optional<std::string>>(std::nullopt);
    dto.cv_pdf = json["cv_pdf"].As<std::optional<std::string>>(std::nullopt);
    return dto;
}

struct ResponseDTO {
    boost::uuids::uuid id;
    boost::uuids::uuid user_id;
    std::string title;
    std::optional<std::string> description;
    std::optional<std::string> cv_pdf;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
};

inline auto Serialize(const ResponseDTO& dto,
                      userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = dto.id;
    builder["user_id"] = dto.user_id;
    builder["title"] = dto.title;
    builder["description"] = dto.description;
    builder["cv_pdf"] = dto.cv_pdf;
    builder["created_at"] = dto.created_at;
    builder["updated_at"] = dto.updated_at;
    return builder.ExtractValue();
}

}  // namespace internview::dto::cv

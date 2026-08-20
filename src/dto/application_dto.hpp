#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>

#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/parse/boost_uuid.hpp"
#include "userver/formats/parse/common_containers.hpp"
#include "userver/formats/parse/to.hpp"
#include "userver/formats/serialize/boost_uuid.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/formats/serialize/to.hpp"

namespace internview::dto::application {

struct CreateDTO {
    boost::uuids::uuid vacancy_id;
    boost::uuids::uuid intern_id;
    boost::uuids::uuid cv_id;
    std::optional<std::string> cover_letter;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<CreateDTO>) {
    CreateDTO dto;
    dto.vacancy_id = json["vacancy_id"].As<boost::uuids::uuid>();
    dto.cover_letter = json["cover_letter"].As<std::optional<std::string>>(std::nullopt);
    dto.cv_id = json["cv_id"].As<boost::uuids::uuid>();
    return dto;
}

struct UpdateDTO {
    boost::uuids::uuid id;
    bool has_cv_id_in_request_json = false;
    bool has_cover_letter_in_request_json = false;
    bool has_status_in_request_json = false;
    boost::uuids::uuid cv_id;
    std::optional<std::string> cover_letter;
    std::string status;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<UpdateDTO>) {
    UpdateDTO dto;
    if (json.HasMember("cv_id")) {
        dto.has_cv_id_in_request_json = true;
        dto.cv_id = json["cv_id"].As<boost::uuids::uuid>();
    }
    if (json.HasMember("cover_letter")) {
        dto.has_cover_letter_in_request_json = true;
        dto.cover_letter = json["cover_letter"].As<std::optional<std::string>>(std::nullopt);
    }
    if (json.HasMember("status")) {
        dto.has_status_in_request_json = true;
        dto.status = json["status"].As<std::string>();
    }
    return dto;
}

}  // namespace internview::dto::application

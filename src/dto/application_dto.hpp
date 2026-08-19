#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>

#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/parse/common_containers.hpp"
#include "userver/formats/parse/to.hpp"
#include "userver/formats/serialize/boost_uuid.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/formats/serialize/to.hpp"
#include "userver/formats/parse/boost_uuid.hpp"

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

}  // namespace internview::dto::application

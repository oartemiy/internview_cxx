#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/boost_uuid.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <vector>

#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/parse/to.hpp"

namespace internview::dto::vacancy {

struct CreateDTO {
    boost::uuids::uuid recruiter_id;
    std::string title;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> requirements;
    std::optional<std::string> salary_range;
    std::optional<std::string> location;
    std::optional<std::string> work_mode;
    std::optional<std::string> experience_level;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<CreateDTO>) {
    CreateDTO dto;
    dto.title = json["title"].As<std::string>();
    dto.description = json["description"].As<std::optional<std::string>>(std::nullopt);
    dto.requirements =
        json["requirements"].As<std::optional<std::vector<std::string>>>(std::nullopt);
    dto.salary_range = json["salary_range"].As<std::optional<std::string>>(std::nullopt);
    dto.location = json["location"].As<std::optional<std::string>>(std::nullopt);
    dto.work_mode = json["work_mode"].As<std::optional<std::string>>(std::nullopt);
    dto.experience_level = json["experience_level"].As<std::optional<std::string>>(std::nullopt);
    return dto;
}

}  // namespace internview::dto::vacancy

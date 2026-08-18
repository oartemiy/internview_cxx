#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <vector>

#include "dto/cv_dto.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/serialize/boost_uuid.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/formats/serialize/to.hpp"
#include "userver/storages/postgres/io/io_fwd.hpp"
#include "userver/storages/postgres/io/pg_types.hpp"

namespace internview::models {

struct Vacancy {
    boost::uuids::uuid id;
    boost::uuids::uuid recruiter_id;
    std::string title;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> requirements;
    std::optional<std::string> salary_range;
    std::optional<std::string> location;
    std::optional<std::string> work_mode;
    std::optional<std::string> experience_level;
    bool is_active;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
};

inline auto Serialize(const Vacancy& vacancy,
                      userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = vacancy.id;
    builder["recruiter_id"] = vacancy.recruiter_id;
    builder["title"] = vacancy.title;
    builder["description"] = vacancy.description;
    builder["requirements"] = vacancy.requirements;
    builder["salary_range"] = vacancy.salary_range;
    builder["location"] = vacancy.location;
    builder["work_mode"] = vacancy.work_mode;
    builder["experience_level"] = vacancy.experience_level;
    builder["is_active"] = vacancy.is_active;
    builder["created_at"] = vacancy.created_at;
    builder["updated_at"] = vacancy.updated_at;
    return builder.ExtractValue();
}

}  // namespace internview::models

namespace userver::storages::postgres::io {
template <>
struct CppToUserPg<internview::models::Vacancy> {
    static constexpr DBTypeName postgres_name = "internview_schema.vacancies";  // NOLINT
};
}  // namespace userver::storages::postgres::io

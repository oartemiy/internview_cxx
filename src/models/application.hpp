#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>

#include "dto/cv_dto.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/serialize/boost_uuid.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/formats/serialize/to.hpp"
#include "userver/storages/postgres/io/io_fwd.hpp"
#include "userver/storages/postgres/io/pg_types.hpp"

namespace internview::models {

struct Application {
    boost::uuids::uuid id;
    boost::uuids::uuid vacancy_id;
    boost::uuids::uuid intern_id;
    boost::uuids::uuid cv_id;
    std::string status;
    std::optional<std::string> cover_letter;
    std::chrono::system_clock::time_point applied_at;
    std::chrono::system_clock::time_point updated_at;
};

inline auto Serialize(const Application& application,
                      userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = application.id;
    builder["vacancy_id"] = application.vacancy_id;
    builder["intern_id"] = application.intern_id;
    builder["cv_id"] = application.cv_id;
    builder["status"] = application.status;
    builder["cover_letter"] = application.cover_letter;
    builder["applied_at"] = application.applied_at;
    builder["updated_at"] = application.updated_at;
    return builder.ExtractValue();
}

}  // namespace internview::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<internview::models::Application> {
    static constexpr DBTypeName postgres_name = "internview_schema.applications";  // NOLINT
};

}  // namespace userver::storages::postgres::io

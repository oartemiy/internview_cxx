#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>

#include "userver/storages/postgres/io/io_fwd.hpp"
#include "userver/storages/postgres/io/pg_types.hpp"

namespace internview::models {

struct CV {
    boost::uuids::uuid id;
    boost::uuids::uuid user_id;
    std::string title;
    std::optional<std::string> description;
    std::optional<std::string> cv_pdf;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
};

}  // namespace internview::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<internview::models::CV> {
    static constexpr DBTypeName postgres_name = "internview_schema.cv";  // NOLINT
};

}  // namespace userver::storages::postgres::io

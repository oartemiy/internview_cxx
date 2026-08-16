#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>

#include "dto/cv_dto.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/serialize/boost_uuid.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/formats/serialize/to.hpp"
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

    internview::dto::cv::ResponseDTO ToResponseDTO() const {
        dto::cv::ResponseDTO res;
        res.id = id;
        res.user_id = user_id;
        res.title = title;
        res.description = description;
        res.cv_pdf = cv_pdf;
        res.created_at = created_at;
        res.updated_at = updated_at;
        return res;
    }
};

inline auto Serialize(const CV& user,
                      userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = user.id;
    builder["user_id"] = user.user_id;
    builder["title"] = user.title;
    builder["description"] = user.description;
    builder["cv_pdf"] = user.cv_pdf;
    builder["created_at"] = user.created_at;
    builder["updated_at"] = user.updated_at;
    return builder.ExtractValue();
}

}  // namespace internview::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<internview::models::CV> {
    static constexpr DBTypeName postgres_name = "internview_schema.cvs";  // NOLINT
};

}  // namespace userver::storages::postgres::io

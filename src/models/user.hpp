#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <userver/formats/serialize/boost_uuid.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/storages/postgres/io/ip.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>

namespace internview::models {

struct User {
    boost::uuids::uuid id;
    std::string login;
    std::string password_hash;
    std::string role;
    std::string name;
    std::optional<std::string> profile_pic;
    std::optional<std::string> description;
    std::chrono::system_clock::time_point created_at;
};

}  // namespace internview::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<internview::models::User> {
    static constexpr DBTypeName postgres_name = "internview_schema.users";  // NOLINT
};

}  // namespace userver::storages::postgres::io

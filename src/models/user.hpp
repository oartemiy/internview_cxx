#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <userver/formats/serialize/boost_uuid.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/storages/postgres/io/ip.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>

#include "dto/user_dto.hpp"

namespace internview::models {

struct User {
    boost::uuids::uuid id;
    std::string login;
    std::string password_hash;
    std::string name;
    std::string role;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
    std::chrono::system_clock::time_point created_at;

    internview::dto::user::ResponseDTO ToResponseDTO() const {
        internview::dto::user::ResponseDTO resp;
        resp.created_at = created_at;
        resp.profile_pic = profile_pic;
        resp.description = description;
        resp.role = role;
        resp.name = name;
        resp.login = login;
        resp.id = id;
        return resp;
    }
};

}  // namespace internview::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<internview::models::User> {
    static constexpr DBTypeName postgres_name = "internview_schema.users";  // NOLINT
};

}  // namespace userver::storages::postgres::io

#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include "userver/storages/postgres/io/io_fwd.hpp"
#include "userver/storages/postgres/io/pg_types.hpp"

namespace internview::models {

struct RefreshToken {
    boost::uuids::uuid id;
    boost::uuids::uuid user_id;
    std::string token_hash;
    bool revoked;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point expires_at;
};

}  // namespace internview::models

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<internview::models::RefreshToken> {
    static constexpr DBTypeName postgres_name = "internview_schema.refresh_tokens";  // NOLINT
};

}
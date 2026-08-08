#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>

#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/boost_uuid.hpp>

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

    userver::formats::json::Value ToJSON() const {
        userver::formats::json::ValueBuilder json_builder;
        json_builder["id"] = id;
        json_builder["login"] = login;
        json_builder["role"] = role;
        json_builder["name"] = name;
        json_builder["profile_pic"] = profile_pic;
        json_builder["description"] = description;
        json_builder["created_at"] = created_at;
        auto json = json_builder.ExtractValue();
        return json;
    }
};

}  // namespace internview::models


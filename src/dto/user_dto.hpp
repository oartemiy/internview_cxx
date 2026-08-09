#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <userver/formats/json.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/formats/serialize/boost_uuid.hpp>

namespace internview::dto::user {

struct CreateDTO {
    std::string login;
    std::string password;
    std::string name;
    std::string role;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
};

struct UpdateDTO {
    // ! role can not be changed
    std::optional<std::string> login;
    std::optional<std::string> password;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
};

struct DeleteDTO {
    std::string login;
    std::string password;
};

struct LoginDTO {
    std::string login;
    std::string password;
};

struct ResponseDTO {
    boost::uuids::uuid id;
    std::string login;
    std::string name;
    std::string role;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
    std::chrono::system_clock::time_point created_at;
    std::string token;

    userver::formats::json::Value ToJSON() const {
        userver::formats::json::ValueBuilder json_builder;
        json_builder["id"] = id;
        json_builder["login"] = login;
        json_builder["name"] = name;
        json_builder["role"] = role;
        json_builder["description"] = description;
        json_builder["profile_pic"] = profile_pic;
        json_builder["created_at"] = created_at;
        json_builder["token"] = token;
        auto json = json_builder.ExtractValue();
        return json;
    }
};
}  // namespace internview::dto::user

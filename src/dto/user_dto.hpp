#pragma once

#include <boost/uuid.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/boost_uuid.hpp>
#include <userver/formats/serialize/common_containers.hpp>

#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/parse/to.hpp"

namespace internview::dto::user {

struct CreateDTO {
    std::string login;
    std::string password;
    std::string name;
    std::string role;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
};

inline auto Parse(const userver::formats::json::Value& json, userver::formats::parse::To<CreateDTO>) {
    CreateDTO dto;
    dto.login = json["login"].As<std::string>();
    dto.password = json["password"].As<std::string>();
    dto.name = json["name"].As<std::string>();
    dto.description = json["description"].As<std::optional<std::string>>(std::nullopt);
    dto.profile_pic = json["profile_pic"].As<std::optional<std::string>>(std::nullopt);
    dto.role = json["role"].As<std::string>();
    return dto;
}

struct UpdateDTO {
    // ! role can not be changed
    // ! changes only public info
    std::optional<std::string> login;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
};

inline auto Parse(const userver::formats::json::Value& json, userver::formats::parse::To<UpdateDTO>) {
    UpdateDTO dto;
    dto.login = json["login"].As<std::optional<std::string>>(std::nullopt);
    dto.name = json["name"].As<std::optional<std::string>>(std::nullopt);
    dto.description = json["description"].As<std::optional<std::string>>(std::nullopt);
    dto.profile_pic = json["profile_pic"].As<std::optional<std::string>>(std::nullopt);
    return dto;
}

struct DeleteDTO {
    std::string login;
    std::string password;
};

inline auto Parse(const userver::formats::json::Value& json, userver::formats::parse::To<DeleteDTO>) {
    DeleteDTO dto;
    dto.login = json["login"].As<std::string>();
    dto.password = json["password"].As<std::string>();
    return dto;
}

struct LoginDTO {
    std::string login;
    std::string password;
};

inline auto Parse(const userver::formats::json::Value& json, userver::formats::parse::To<LoginDTO>) {
    LoginDTO dto;
    dto.login = json["login"].As<std::string>();
    dto.password = json["password"].As<std::string>();
    return dto;
}

struct ResponseDTO {
    boost::uuids::uuid id;
    std::string login;
    std::string name;
    std::string role;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
    std::chrono::system_clock::time_point created_at;
    std::string token;
};

inline auto Serialize(const ResponseDTO& dto,
               userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = dto.id;
    builder["login"] = dto.login;
    builder["name"] = dto.name;
    builder["role"] = dto.role;
    builder["description"] = dto.description;
    builder["profile_pic"] = dto.profile_pic;
    builder["created_at"] = dto.created_at;
    builder["token"] = dto.token;
    auto json = builder.ExtractValue();
    return json;
}

}  // namespace internview::dto::user

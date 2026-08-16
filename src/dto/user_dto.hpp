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

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<CreateDTO>) {
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
    bool has_login_in_request = false;
    bool has_name_in_request = false;
    bool has_description_in_request = false;
    bool has_profile_pic_in_request = false;
    boost::uuids::uuid id;
    std::string login;
    std::string name;
    std::optional<std::string> description;
    std::optional<std::string> profile_pic;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<UpdateDTO>) {
    UpdateDTO dto;
    if (json.HasMember("login")) {
        dto.has_login_in_request = true;
        dto.login = json["login"].As<std::string>();
    }
    if (json.HasMember("name")) {
        dto.has_name_in_request = true;
        dto.name = json["name"].As<std::string>();
    }
    if (json.HasMember("description")) {
        dto.has_description_in_request = true;
        dto.description = json["description"].As<std::optional<std::string>>(std::nullopt);
    }
    if (json.HasMember("profile_pic")) {
        dto.has_profile_pic_in_request = true;
        dto.profile_pic = json["profile_pic"].As<std::optional<std::string>>(std::nullopt);
    }
    return dto;
}

struct DeleteDTO {
    boost::uuids::uuid id;
    std::string login;
    std::string password;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<DeleteDTO>) {
    DeleteDTO dto;
    dto.login = json["login"].As<std::string>();
    dto.password = json["password"].As<std::string>();
    return dto;
}

struct LoginDTO {
    std::string login;
    std::string password;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<LoginDTO>) {
    LoginDTO dto;
    dto.login = json["login"].As<std::string>();
    dto.password = json["password"].As<std::string>();
    return dto;
}

struct ChangePasswordDTO {
    boost::uuids::uuid id;
    std::string old_password;
    std::string new_password;
};

inline auto Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<ChangePasswordDTO>) {
    ChangePasswordDTO dto;
    dto.new_password = json["new_password"].As<std::string>();
    dto.old_password = json["old_password"].As<std::string>();
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
    std::optional<std::string> token;
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
    if (dto.token) {
        builder["token"] = dto.token;
    }
    auto json = builder.ExtractValue();
    return json;
}

}  // namespace internview::dto::user

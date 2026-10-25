#include <userver/formats/json/exception.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utest/utest.hpp>
#include <userver/utils/boost_uuid7.hpp>

#include "dto/application_dto.hpp"
#include "dto/cv_dto.hpp"
#include "dto/user_dto.hpp"
#include "dto/vacancy_dto.hpp"

namespace {

using userver::formats::json::FromString;
using JsonException = userver::formats::json::Exception;

}  // namespace

// ---------------------------------------------------------------- user

TEST(UserDto, ParsesRegisterWithAllFields) {
    const auto json = FromString(R"({
        "login": "alice", "password": "secret", "name": "Alice", "role": "intern",
        "description": "about me", "profile_pic": "pic.png"
    })");

    const auto dto = json.As<internview::dto::user::RegisterDTO>();

    EXPECT_EQ(dto.login, "alice");
    EXPECT_EQ(dto.password, "secret");
    EXPECT_EQ(dto.name, "Alice");
    EXPECT_EQ(dto.role, "intern");
    EXPECT_EQ(dto.description, "about me");
    // file storage keys are never taken from the request
    EXPECT_EQ(dto.profile_pic, std::nullopt);
}

TEST(UserDto, RegisterOptionalFieldsDefaultToNullopt) {
    const auto json =
        FromString(R"({"login": "alice", "password": "secret", "name": "A", "role": "intern"})");

    const auto dto = json.As<internview::dto::user::RegisterDTO>();

    EXPECT_EQ(dto.description, std::nullopt);
    EXPECT_EQ(dto.profile_pic, std::nullopt);
}

TEST(UserDto, RegisterThrowsOnMissingRequiredField) {
    const auto json = FromString(R"({"login": "alice", "name": "A", "role": "intern"})");

    EXPECT_THROW(json.As<internview::dto::user::RegisterDTO>(), JsonException);
}

TEST(UserDto, RegisterThrowsOnWrongType) {
    const auto json =
        FromString(R"({"login": 42, "password": "secret", "name": "A", "role": "intern"})");

    EXPECT_THROW(json.As<internview::dto::user::RegisterDTO>(), JsonException);
}

TEST(UserDto, UpdateMarksOnlyPresentFields) {
    const auto json = FromString(R"({"name": "New name"})");

    const auto dto = json.As<internview::dto::user::UpdateDTO>();

    EXPECT_TRUE(dto.has_name_in_request);
    EXPECT_EQ(dto.name, "New name");
    EXPECT_FALSE(dto.has_login_in_request);
    EXPECT_FALSE(dto.has_description_in_request);
    EXPECT_FALSE(dto.has_profile_pic_in_request);
}

TEST(UserDto, UpdateTreatsExplicitNullAsPresentField) {
    const auto json = FromString(R"({"description": null})");

    const auto dto = json.As<internview::dto::user::UpdateDTO>();

    EXPECT_TRUE(dto.has_description_in_request);
    EXPECT_EQ(dto.description, std::nullopt);
}

TEST(UserDto, ParsesLoginDeleteAndChangePassword) {
    const auto login =
        FromString(R"({"login": "a", "password": "p"})").As<internview::dto::user::LoginDTO>();
    EXPECT_EQ(login.login, "a");
    EXPECT_EQ(login.password, "p");

    const auto del =
        FromString(R"({"login": "a", "password": "p"})").As<internview::dto::user::DeleteDTO>();
    EXPECT_EQ(del.login, "a");
    EXPECT_EQ(del.password, "p");

    const auto change = FromString(R"({"old_password": "o", "new_password": "n"})")
                            .As<internview::dto::user::ChangePasswordDTO>();
    EXPECT_EQ(change.old_password, "o");
    EXPECT_EQ(change.new_password, "n");
}

TEST(UserDto, ResponseOmitsTokensWhenAbsent) {
    internview::dto::user::ResponseDTO dto;
    dto.id = userver::utils::generators::GenerateBoostUuidV7();
    dto.login = "alice";
    dto.name = "Alice";
    dto.role = "intern";

    const auto json = userver::formats::json::ValueBuilder(dto).ExtractValue();

    EXPECT_EQ(json["id"].As<std::string>(), boost::uuids::to_string(dto.id));
    EXPECT_EQ(json["login"].As<std::string>(), "alice");
    EXPECT_TRUE(json["description"].IsNull());
    EXPECT_FALSE(json.HasMember("access_token"));
    EXPECT_FALSE(json.HasMember("refresh_token"));
    EXPECT_FALSE(json.HasMember("password"));
    EXPECT_FALSE(json.HasMember("password_hash"));
}

TEST(UserDto, ResponseContainsTokensWhenPresent) {
    internview::dto::user::ResponseDTO dto;
    dto.id = userver::utils::generators::GenerateBoostUuidV7();
    dto.access_token = "access";
    dto.refresh_token = "refresh";

    const auto json = userver::formats::json::ValueBuilder(dto).ExtractValue();

    EXPECT_EQ(json["access_token"].As<std::string>(), "access");
    EXPECT_EQ(json["refresh_token"].As<std::string>(), "refresh");
}

// ---------------------------------------------------------------- cv

TEST(CvDto, ParsesCreate) {
    const auto dto = FromString(R"({"title": "Backend", "description": "C++"})")
                         .As<internview::dto::cv::CreateDTO>();

    EXPECT_EQ(dto.title, "Backend");
    EXPECT_EQ(dto.description, "C++");
    EXPECT_EQ(dto.cv_pdf, std::nullopt);
}

TEST(CvDto, CreateIgnoresCvPdfKey) {
    const auto dto = FromString(R"({"title": "Backend", "cv_pdf": "../../etc/passwd"})")
                         .As<internview::dto::cv::CreateDTO>();

    EXPECT_EQ(dto.cv_pdf, std::nullopt);
}

TEST(CvDto, CreateThrowsWithoutTitle) {
    EXPECT_THROW(FromString(R"({"description": "C++"})").As<internview::dto::cv::CreateDTO>(),
                 JsonException);
}

TEST(CvDto, UpdateMarksOnlyPresentFields) {
    const auto dto =
        FromString(R"({"title": "New", "cv_pdf": null})").As<internview::dto::cv::UpdateDTO>();

    EXPECT_TRUE(dto.has_title_in_request);
    EXPECT_EQ(dto.title, "New");
    EXPECT_TRUE(dto.has_cv_pdf_in_request);
    EXPECT_EQ(dto.cv_pdf, std::nullopt);
    EXPECT_FALSE(dto.has_description_in_request);
}

// ---------------------------------------------------------------- vacancy

TEST(VacancyDto, ParsesCreateWithRequirements) {
    const auto dto = FromString(R"({
        "title": "Intern C++", "requirements": ["C++20", "SQL"], "work_mode": "remote"
    })")
                         .As<internview::dto::vacancy::CreateDTO>();

    EXPECT_EQ(dto.title, "Intern C++");
    ASSERT_TRUE(dto.requirements.has_value());
    EXPECT_EQ(*dto.requirements, (std::vector<std::string>{"C++20", "SQL"}));
    EXPECT_EQ(dto.work_mode, "remote");
    EXPECT_EQ(dto.location, std::nullopt);
    EXPECT_EQ(dto.salary_range, std::nullopt);
    EXPECT_EQ(dto.experience_level, std::nullopt);
}

TEST(VacancyDto, CreateThrowsOnNonArrayRequirements) {
    EXPECT_THROW(FromString(R"({"title": "T", "requirements": "C++"})")
                     .As<internview::dto::vacancy::CreateDTO>(),
                 JsonException);
}

TEST(VacancyDto, UpdateMarksOnlyPresentFields) {
    const auto dto = FromString(R"({"location": "Moscow", "salary_range": null})")
                         .As<internview::dto::vacancy::UpdateDTO>();

    EXPECT_TRUE(dto.has_location_in_request_json);
    EXPECT_EQ(dto.location, "Moscow");
    EXPECT_TRUE(dto.has_salary_range_in_request_json);
    EXPECT_EQ(dto.salary_range, std::nullopt);
    EXPECT_FALSE(dto.has_title_in_request_json);
    EXPECT_FALSE(dto.has_description_in_request_json);
    EXPECT_FALSE(dto.has_requirements_in_request_json);
    EXPECT_FALSE(dto.has_work_mode_in_request_json);
    EXPECT_FALSE(dto.has_experience_level_in_request_json);
}

TEST(VacancyDto, GetHasSaneDefaults) {
    const internview::dto::vacancy::GetDTO dto;

    EXPECT_EQ(dto.limit, 15);
    EXPECT_EQ(dto.offset, 0);
    EXPECT_EQ(dto.query, std::nullopt);
}

// ---------------------------------------------------------------- application

TEST(ApplicationDto, ParsesCreate) {
    const auto vacancy_id = userver::utils::generators::GenerateBoostUuidV7();
    const auto cv_id = userver::utils::generators::GenerateBoostUuidV7();
    userver::formats::json::ValueBuilder builder;
    builder["vacancy_id"] = vacancy_id;
    builder["cv_id"] = cv_id;
    builder["cover_letter"] = "hello";

    const auto dto = builder.ExtractValue().As<internview::dto::application::CreateDTO>();

    EXPECT_EQ(dto.vacancy_id, vacancy_id);
    EXPECT_EQ(dto.cv_id, cv_id);
    EXPECT_EQ(dto.cover_letter, "hello");
}

TEST(ApplicationDto, CreateThrowsOnInvalidUuid) {
    EXPECT_ANY_THROW(FromString(R"({"vacancy_id": "not-uuid", "cv_id": "not-uuid"})")
                         .As<internview::dto::application::CreateDTO>());
}

TEST(ApplicationDto, UpdateMarksOnlyPresentFields) {
    const auto dto =
        FromString(R"({"status": "approved"})").As<internview::dto::application::UpdateDTO>();

    EXPECT_TRUE(dto.has_status_in_request_json);
    EXPECT_EQ(dto.status, "approved");
    EXPECT_FALSE(dto.has_cv_id_in_request_json);
    EXPECT_FALSE(dto.has_cover_letter_in_request_json);
}

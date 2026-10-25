#include <userver/formats/json/value_builder.hpp>
#include <userver/utest/utest.hpp>
#include <userver/utils/boost_uuid7.hpp>

#include "models/application.hpp"
#include "models/cv.hpp"
#include "models/user.hpp"
#include "models/vacancy.hpp"

namespace {

using userver::utils::generators::GenerateBoostUuidV7;

}  // namespace

TEST(UserModel, ToResponseDtoCopiesPublicFieldsOnly) {
    internview::models::User user;
    user.id = GenerateBoostUuidV7();
    user.login = "alice";
    user.password_hash = "$argon2id$secret";
    user.name = "Alice";
    user.role = "intern";
    user.description = "about";
    user.profile_pic = std::nullopt;
    user.created_at = std::chrono::system_clock::now();
    user.password_version = 7;

    const auto dto = user.ToResponseDTO();

    EXPECT_EQ(dto.id, user.id);
    EXPECT_EQ(dto.login, "alice");
    EXPECT_EQ(dto.name, "Alice");
    EXPECT_EQ(dto.role, "intern");
    EXPECT_EQ(dto.description, "about");
    EXPECT_EQ(dto.created_at, user.created_at);
    EXPECT_EQ(dto.access_token, std::nullopt);
    EXPECT_EQ(dto.refresh_token, std::nullopt);

    const auto json = userver::formats::json::ValueBuilder(dto).ExtractValue();
    EXPECT_FALSE(json.HasMember("password_hash"));
    EXPECT_FALSE(json.HasMember("password_version"));
}

TEST(VacancyModel, SerializesAllFields) {
    internview::models::Vacancy vacancy;
    vacancy.id = GenerateBoostUuidV7();
    vacancy.recruiter_id = GenerateBoostUuidV7();
    vacancy.title = "C++ intern";
    vacancy.description = std::nullopt;
    vacancy.requirements = std::vector<std::string>{"C++", "Linux"};
    vacancy.salary_range = "100-200";
    vacancy.location = std::nullopt;
    vacancy.work_mode = "remote";
    vacancy.experience_level = std::nullopt;
    vacancy.is_active = true;
    vacancy.created_at = std::chrono::system_clock::now();
    vacancy.updated_at = vacancy.created_at;

    const auto json = userver::formats::json::ValueBuilder(vacancy).ExtractValue();

    EXPECT_EQ(json["id"].As<std::string>(), boost::uuids::to_string(vacancy.id));
    EXPECT_EQ(json["recruiter_id"].As<std::string>(), boost::uuids::to_string(vacancy.recruiter_id));
    EXPECT_EQ(json["title"].As<std::string>(), "C++ intern");
    EXPECT_TRUE(json["description"].IsNull());
    EXPECT_EQ(json["requirements"].As<std::vector<std::string>>(),
              (std::vector<std::string>{"C++", "Linux"}));
    EXPECT_EQ(json["salary_range"].As<std::string>(), "100-200");
    EXPECT_TRUE(json["location"].IsNull());
    EXPECT_EQ(json["work_mode"].As<std::string>(), "remote");
    EXPECT_TRUE(json["is_active"].As<bool>());
    EXPECT_TRUE(json.HasMember("created_at"));
    EXPECT_TRUE(json.HasMember("updated_at"));
}

TEST(CvModel, SerializesAllFields) {
    internview::models::CV cv;
    cv.id = GenerateBoostUuidV7();
    cv.user_id = GenerateBoostUuidV7();
    cv.title = "Backend";
    cv.description = "C++";
    cv.cv_pdf = std::nullopt;
    cv.created_at = std::chrono::system_clock::now();
    cv.updated_at = cv.created_at;

    const auto json = userver::formats::json::ValueBuilder(cv).ExtractValue();

    EXPECT_EQ(json["id"].As<std::string>(), boost::uuids::to_string(cv.id));
    EXPECT_EQ(json["user_id"].As<std::string>(), boost::uuids::to_string(cv.user_id));
    EXPECT_EQ(json["title"].As<std::string>(), "Backend");
    EXPECT_EQ(json["description"].As<std::string>(), "C++");
    EXPECT_TRUE(json["cv_pdf"].IsNull());
}

TEST(ApplicationModel, SerializesAllFields) {
    internview::models::Application application;
    application.id = GenerateBoostUuidV7();
    application.vacancy_id = GenerateBoostUuidV7();
    application.intern_id = GenerateBoostUuidV7();
    application.cv_id = GenerateBoostUuidV7();
    application.status = "pending";
    application.cover_letter = std::nullopt;
    application.applied_at = std::chrono::system_clock::now();
    application.updated_at = application.applied_at;

    const auto json = userver::formats::json::ValueBuilder(application).ExtractValue();

    EXPECT_EQ(json["id"].As<std::string>(), boost::uuids::to_string(application.id));
    EXPECT_EQ(json["vacancy_id"].As<std::string>(),
              boost::uuids::to_string(application.vacancy_id));
    EXPECT_EQ(json["intern_id"].As<std::string>(), boost::uuids::to_string(application.intern_id));
    EXPECT_EQ(json["cv_id"].As<std::string>(), boost::uuids::to_string(application.cv_id));
    EXPECT_EQ(json["status"].As<std::string>(), "pending");
    EXPECT_TRUE(json["cover_letter"].IsNull());
    EXPECT_TRUE(json.HasMember("applied_at"));
    EXPECT_TRUE(json.HasMember("updated_at"));
}

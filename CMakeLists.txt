cmake_minimum_required(VERSION 3.12...3.31)
project(internview CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)
include(DownloadSodium)
include(DownloadJwt-cpp)

find_package(
    userver
    COMPONENTS core #
    postgresql
    QUIET
)
if(NOT userver_FOUND)
    # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
    download_userver(GIT_TAG v3.1)
endif()

userver_setup_environment()

# Common sources
include_directories(src)

# UserStorage queries
userver_add_sql_library(
    ${PROJECT_NAME}_user_storage_sql
    NAMESPACE
    user_storage_queries
    SOURCE_DIR
    src/storages/sql/user_storage
    OUTPUT_DIR
    ${CMAKE_SOURCE_DIR}/src/storages/sql/user_storage
    SQL_FILES
    *.sql
)

userver_add_sql_library(
    ${PROJECT_NAME}_cv_storage_sql
    NAMESPACE
    cv_storage_queries
    SOURCE_DIR
    src/storages/sql/cv_storage
    OUTPUT_DIR
    ${CMAKE_SOURCE_DIR}/src/storages/sql/cv_storage
    SQL_FILES
    *.sql
)

userver_add_sql_library(
    ${PROJECT_NAME}_vacancy_storage_sql
    NAMESPACE
    vacancy_storage_queries
    SOURCE_DIR
    src/storages/sql/vacancy_storage
    OUTPUT_DIR
    ${CMAKE_SOURCE_DIR}/src/storages/sql/vacancy_storage
    SQL_FILES
    *.sql
)


target_include_directories(${PROJECT_NAME}_user_storage_sql PUBLIC ${CMAKE_SOURCE_DIR}/src/storages/sql/user_storage/include)
target_include_directories(${PROJECT_NAME}_cv_storage_sql PUBLIC ${CMAKE_SOURCE_DIR}/src/storages/sql/cv_storage/include)
target_include_directories(${PROJECT_NAME}_vacancy_storage_sql PUBLIC ${CMAKE_SOURCE_DIR}/src/storages/sql/vacancy_storage/include)


add_library(
    ${PROJECT_NAME}_objs OBJECT
    # utils
    src/utils/password.cpp

    # services
    src/services/jwt_service.cpp
    src/services/file_service.cpp
    src/services/auth_service.cpp

    # components
    src/components/internview_component.cpp

    # stotages
    src/storages/user_storage.cpp
    src/storages/cv_storage.cpp
    src/storages/vacancy_storage.cpp


    # handlers

    # status
    src/handlers/status/handler_status_get.cpp


    # auth
    src/handlers/auth/handler_auth_register_post.cpp
    src/handlers/auth/handler_auth_login_post.cpp
    src/handlers/auth/handler_auth_change_password_post.cpp


    # user
    src/handlers/user/handler_user_delete.cpp
    src/handlers/user/handler_user_get.cpp
    src/handlers/user/handler_user_update.cpp
    src/handlers/files/handler_profile_pic_post.cpp
    src/handlers/files/handler_profile_pic_get.cpp

    # cv
    src/handlers/cv/handler_cv_post.cpp
    src/handlers/cv/handler_cv_get.cpp
    src/handlers/cv/handler_cv_get_by_id.cpp
    src/handlers/cv/handler_cv_update.cpp
    src/handlers/cv/handler_cv_delete.cpp
    src/handlers/files/handler_cv_pdf_post.cpp
    src/handlers/files/handler_cv_pdf_get.cpp

    # vacancy
    src/handlers/vacancy/handler_vacancy_post.cpp
    src/handlers/vacancy/handler_vacancies_get.cpp
    src/handlers/vacancy/handler_vacancies_get_by_id.cpp
    src/handlers/vacancy/handler_vacancies_get_me.cpp
    src/handlers/vacancy/handler_vacancies_update_id.cpp
    src/handlers/vacancy/handler_vacancies_delete.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_objs
    PUBLIC userver::core #
    userver::postgresql
    ${PROJECT_NAME}_user_storage_sql
    ${PROJECT_NAME}_cv_storage_sql
    ${PROJECT_NAME}_vacancy_storage_sql

    PRIVATE
    sodium
    jwt-cpp::jwt-cpp
)



# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# # Unit Tests
# add_executable(${PROJECT_NAME}_unittest src/greeting_test.cpp)
# target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
# add_google_tests(${PROJECT_NAME}_unittest)

# # Benchmarks
# add_executable(${PROJECT_NAME}_benchmark src/greeting_benchmark.cpp)
# target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
# add_google_benchmark_tests(${PROJECT_NAME}_benchmark)

# Functional testing
userver_testsuite_add_simple()

# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
    message(STATUS "Set install prefix: $ENV{PREFIX}")
    file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
    set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
)
install(
    FILES ${CONFIGS_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)

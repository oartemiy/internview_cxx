include_guard(GLOBAL)
include(get_cpm)

find_package(jwt-cpp QUIET)

if(NOT jwt-cpp_FOUND)
    cpmaddpackage(
        NAME
        jwt-cpp
        GIT_REPOSITORY
        https://github.com/Thalhammer/jwt-cpp.git
        VERSION
        0.7.2
    )
endif()

include_guard(GLOBAL)
include(get_cpm)

find_package(sodium QUIET)

if(NOT sodium_FOUND)
    cpmaddpackage(
        NAME
        sodium
        GIT_REPOSITORY
        https://github.com/robinlinden/libsodium-cmake.git
        GIT_TAG
        cfebfd3da486d5a86c644c8b47067e5411c7599c
        # OPTIONS 
        # "SODIUM_DISABLE_TESTS ON"
    )

    set(SODIUM_DISABLE_TESTS ON)
endif()

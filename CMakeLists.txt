cmake_minimum_required(VERSION 3.16)
project(drip VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    drip/cli/main.cpp
    drip/core/version.cpp
    drip/core/package.cpp
    drip/core/resolver.cpp
    drip/core/fetcher.cpp
    drip/core/builder.cpp
    drip/core/manifest.cpp
    drip/details/git_utils.cpp
    drip/details/cmake_utils.cpp
    drip/cli/cmd/help.cpp
    drip/cli/cmd/init.cpp
    drip/cli/cmd/add.cpp
    drip/cli/cmd/fetch.cpp
    drip/cli/cmd/build.cpp
    drip/cli/cmd/run.cpp
    drip/cli/cmd/install.cpp
    drip/cli/cmd/resolve.cpp
    drip/cli/cmd/lock.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(drip ${SOURCES})
target_include_directories(drip PRIVATE ${INCLUDE_DIRS})

# --- Tests ---
option(DRIP_BUILD_TESTS "Build unit tests" ON)

if(DRIP_BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE TEST_SOURCES "drip/**/*.test.cpp")
    list(REMOVE_ITEM SOURCES "drip/cli/main.cpp")

    add_executable(drip-tests ${SOURCES} ${TEST_SOURCES})
    target_include_directories(drip-tests PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(drip-tests PRIVATE gtest_main gmock)

    include(GoogleTest)
    gtest_discover_tests(drip-tests)
endif()

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

cmake_minimum_required(VERSION 3.16)
project(drip VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    drip/main.cpp
    drip/version.cpp
    drip/package.cpp
    drip/resolver.cpp
    drip/fetcher.cpp
    drip/builder.cpp
    drip/manifest.cpp
    drip/details/git_utils.cpp
    drip/details/cmake_utils.cpp
    drip/cmd/help.cpp
    drip/cmd/init.cpp
    drip/cmd/add.cpp
    drip/cmd/fetch.cpp
    drip/cmd/build.cpp
    drip/cmd/run.cpp
    drip/cmd/install.cpp
    drip/cmd/resolve.cpp
    drip/cmd/lock.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(drip ${SOURCES})
target_include_directories(drip PRIVATE ${INCLUDE_DIRS})

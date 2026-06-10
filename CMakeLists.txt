cmake_minimum_required(VERSION 3.16)
project(drip VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/version.cpp
    src/package.cpp
    src/resolver.cpp
    src/fetcher.cpp
    src/builder.cpp
    src/manifest.cpp
    src/utils/git_utils.cpp
    src/utils/cmake_utils.cpp
    src/cmd/help.cpp
    src/cmd/init.cpp
    src/cmd/add.cpp
    src/cmd/fetch.cpp
    src/cmd/build.cpp
    src/cmd/run.cpp
    src/cmd/install.cpp
    src/cmd/resolve.cpp
    src/cmd/lock.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(drip ${SOURCES})
target_include_directories(drip PRIVATE ${INCLUDE_DIRS})

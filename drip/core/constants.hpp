#pragma once

#include <array>
#include <string_view>

inline constexpr auto PROJECT_DIR = "src";

inline constexpr auto MANIFEST = "drip.toml";
inline constexpr auto LOCKFILE = "drip.lock";
inline constexpr auto CMAKE_CONFIG = "drip-config.cmake";
inline constexpr auto CMAKE_DEPS = "drip-deps.cmake";
inline constexpr auto CMAKE_TEMPLATE = "CMakeLists.txt";
inline constexpr auto SOURCE_TEMPLATE = "src/main.cpp";
inline constexpr auto GIT_SUFFIX = ".git";
inline constexpr auto DEFAULT_PROJECT = "myapp";
inline constexpr auto DEFAULT_VERSION = "0.1.0";
inline constexpr auto FALLBACK_VERSION = "0.0.0";
inline constexpr auto DEPS_HEADER = "[dependencies]";
inline constexpr auto GH_PREFIX = "gh:";
inline constexpr auto GITHUB_PREFIX = "github:";
inline constexpr auto GITHUB_URL = "https://github.com/";

inline constexpr auto CMAKE_CONFIGURE = "cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 2>&1";
inline constexpr auto CMAKE_BUILD = "cmake --build build 2>&1";
inline constexpr auto BINARY_PREFIX = "./build/";

inline constexpr auto TARGETS_FILE_SUFFIX = "targets.cmake";
inline constexpr auto TARGETS_FILE_PATTERN = "targets-";
inline constexpr auto ADD_LIBRARY_PATTERN = "add_library(";
inline constexpr auto CMAKE_SUBDIRS = std::array{std::string_view("lib/cmake"), std::string_view("share/cmake")};
inline constexpr auto DRIP_PACKAGES = "/.drip/packages/";

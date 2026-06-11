#pragma once

#include <drip/core/constants.hpp>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

std::string to_lower(std::string s);
std::vector<std::string> parse_cmake_targets(const std::filesystem::path& dir);

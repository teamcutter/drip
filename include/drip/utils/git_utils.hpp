#pragma once

#include "drip/package.hpp"
#include <string>
#include <string_view>

namespace drip {

GitSource parse_git_ref(std::string_view input, std::string_view rev = {});
std::string extract_pkg_name(std::string_view input);

}

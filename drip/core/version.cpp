#include <drip/core/version.hpp>

namespace drip {

std::optional<Version> Version::parse(std::string_view s) {
    Version v;
    std::string str(s);
    if (!str.empty() && str[0] == 'v') str.erase(0, 1);
    auto pos = str.find('-');
    auto build_pos = str.find('+');
    std::string ver_part = str.substr(0, pos != std::string::npos ? pos : build_pos);

    std::stringstream ss(ver_part);
    std::string segment;
    while (std::getline(ss, segment, '.')) {
        try {
            v.components.push_back(std::stoi(segment));
        } catch (...) {
            return std::nullopt;
        }
    }

    if (v.components.empty()) return std::nullopt;

    if (pos != std::string::npos && (build_pos == std::string::npos || pos < build_pos)) {
        v.prerelease = str.substr(pos + 1, build_pos != std::string::npos ? build_pos - pos - 1 : std::string::npos);
    }
    if (build_pos != std::string::npos) {
        v.build = str.substr(build_pos + 1);
    }

    return v;
}

bool Version::satisfies(std::string_view constraint) const {
    (void)constraint;
    return true;
}

std::string Version::to_string() const {
    std::string result;
    for (size_t i = 0; i < components.size(); i++) {
        if (i > 0) result += ".";
        result += std::to_string(components[i]);
    }
    if (!prerelease.empty()) result += "-" + prerelease;
    if (!build.empty()) result += "+" + build;
    return result;
}

}

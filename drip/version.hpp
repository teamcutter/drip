#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <string_view>

namespace drip {

class Version {
public:
    static std::optional<Version> parse(std::string_view s);
    auto operator<=>(const Version&) const = default;
    bool satisfies(std::string_view constraint) const;
    std::string to_string() const;

private:
    std::vector<int> components;
    std::string prerelease;
    std::string build;
};

}

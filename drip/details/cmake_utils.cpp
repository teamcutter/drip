#include <drip/details/cmake_utils.hpp>
#include <cctype>
#include <fstream>
#include <set>

std::string to_lower(std::string s) {
    for (auto& c : s) c = std::tolower(c);
    return s;
}

std::vector<std::string> parse_cmake_targets(const std::filesystem::path& dir) {
    std::vector<std::string> targets;
    std::set<std::string> seen;

    auto skip = [](std::string_view name) {
        for (auto s : {"_main", "-main", "-header-only", "-header_only", "-static", "-shared"})
            if (name.ends_with(s)) return true;
        return false;
    };

    for (auto& entry : std::filesystem::directory_iterator(dir)) {
        auto name = entry.path().filename().string();
        auto lower = to_lower(name);
        if (!lower.ends_with(TARGETS_FILE_SUFFIX)) continue;
        if (lower.find(TARGETS_FILE_PATTERN) != std::string::npos) continue;

        std::ifstream f(entry.path());
        std::string line;
        while (std::getline(f, line)) {
            auto pos = line.find(ADD_LIBRARY_PATTERN);
            if (pos == std::string::npos) continue;
            auto start = pos + 12;
            auto end = line.find(' ', start);
            if (end == std::string::npos) continue;
            auto t = line.substr(start, end - start);
            if (skip(t)) continue;
            if (seen.insert(t).second)
                targets.push_back(std::move(t));
        }
    }
    return targets;
}

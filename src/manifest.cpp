#include "drip/manifest.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace drip {

namespace {

std::string trim(std::string_view s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos) return {};
    auto end = s.find_last_not_of(" \t\r\n");
    return std::string(s.substr(start, end - start + 1));
}

std::string unquote(std::string_view s) {
    auto t = trim(s);
    if (t.size() >= 2 && t.front() == '"' && t.back() == '"') {
        return t.substr(1, t.size() - 2);
    }
    return t;
}

void parse_inline_table(std::string_view content, ManifestDependency& dep) {
    auto brace_start = content.find('{');
    auto brace_end = content.rfind('}');
    if (brace_start == std::string_view::npos || brace_end == std::string_view::npos) return;

    auto inner = content.substr(brace_start + 1, brace_end - brace_start - 1);
    std::string_view remaining = inner;

    while (!remaining.empty()) {
        auto comma = remaining.find(',');
        auto kv = trim(remaining.substr(0, comma));
        remaining = (comma != std::string_view::npos) ? remaining.substr(comma + 1) : "";

        auto eq = kv.find('=');
        if (eq == std::string_view::npos) continue;

        auto key = trim(kv.substr(0, eq));
        auto val = unquote(kv.substr(eq + 1));

        if (key == "git") dep.git = val;
        else if (key == "rev") dep.rev = val;
        else if (key == "version") {
            if (dep.rev.empty()) dep.rev = val;
        }
    }
}

}

Result<Manifest, std::string> parse_manifest(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return Result<Manifest, std::string>(
            "Failed to open " + path.string());
    }

    Manifest manifest;
    std::string section;
    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;
        auto trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        if (trimmed[0] == '[') {
            auto end_bracket = trimmed.find(']');
            if (end_bracket == std::string::npos) {
                return Result<Manifest, std::string>(
                    "Syntax error line " + std::to_string(line_num) + ": unmatched [");
            }
            section = trim(trimmed.substr(1, end_bracket - 1));
            continue;
        }

        if (section.empty()) {
            return Result<Manifest, std::string>(
                "Syntax error line " + std::to_string(line_num) + ": key=value outside section");
        }

        auto eq = trimmed.find('=');
        if (eq == std::string::npos) continue;

        auto key = trim(trimmed.substr(0, eq));

        if (section == "project") {
            auto val = unquote(trimmed.substr(eq + 1));
            if (key == "name") manifest.project_name = val;
            else if (key == "version") manifest.project_version = val;
        } else if (section == "dependencies") {
            ManifestDependency dep;
            dep.name = key;
            parse_inline_table(trimmed.substr(eq + 1), dep);
            if (!dep.git.empty()) {
                manifest.dependencies.push_back(std::move(dep));
            }
        }
    }

    return Result<Manifest, std::string>(std::move(manifest));
}

Manifest create_template_manifest(const std::string& name) {
    Manifest m;
    m.project_name = name;
    m.project_version = "0.1.0";
    return m;
}

}

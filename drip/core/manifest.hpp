#pragma once

#include <drip/core/result.hpp>
#include <string>
#include <vector>
#include <filesystem>

namespace drip {

struct ManifestDependency {
    std::string name;
    std::string git;
    std::string rev;
};

struct Manifest {
    std::string project_name;
    std::string project_version;
    std::vector<ManifestDependency> dependencies;
};

Result<Manifest, std::string> parse_manifest(const std::filesystem::path& path);
Manifest create_template_manifest(const std::string& name);

}

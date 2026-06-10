#pragma once

#include "result.hpp"
#include "version.hpp"
#include <filesystem>
#include <string>

namespace drip {

struct BuildResult {
    std::filesystem::path install_path;
};

class CMakeBuilder {
public:
    CMakeBuilder();
    explicit CMakeBuilder(std::filesystem::path install_root);

    Result<BuildResult, std::string> build(
        const std::filesystem::path& source_dir,
        const std::string& package_name,
        const Version& version);

    static bool cmake_available();

    static std::filesystem::path default_install_root();
    void set_install_root(std::filesystem::path dir);

    int parallel_jobs() const;
    void set_parallel_jobs(int n);

private:
    std::filesystem::path install_root_;
    int jobs_ = 0;
};

}

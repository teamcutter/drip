#include <drip/core/builder.hpp>
#include <cstdlib>
#include <array>
#include <memory>
#include <functional>
#include <thread>

namespace drip {

namespace {

std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, std::function<void(FILE*)>> pipe(
        popen(cmd.c_str(), "r"),
        [](FILE* f) { pclose(f); });
    if (!pipe) return {};
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    while (!result.empty() && (result.back() == '\n' || result.back() == ' ')) {
        result.pop_back();
    }
    return result;
}

}

CMakeBuilder::CMakeBuilder()
    : install_root_(default_install_root()) {}

CMakeBuilder::CMakeBuilder(std::filesystem::path install_root)
    : install_root_(std::move(install_root)) {}

bool CMakeBuilder::cmake_available() {
    auto out = exec("cmake --version 2>&1");
    return out.find("cmake version") != std::string::npos;
}

std::filesystem::path CMakeBuilder::default_install_root() {
    auto home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".drip" / "packages";
    }
    return std::filesystem::path("/usr") / "local";
}

void CMakeBuilder::set_install_root(std::filesystem::path dir) {
    install_root_ = std::move(dir);
}

int CMakeBuilder::parallel_jobs() const {
    return jobs_ > 0 ? jobs_ : static_cast<int>(std::thread::hardware_concurrency());
}

void CMakeBuilder::set_parallel_jobs(int n) {
    jobs_ = n;
}

Result<BuildResult, std::string> CMakeBuilder::build(
    const std::filesystem::path& source_dir,
    const std::string& package_name,
    const Version& version)
{
    if (!cmake_available()) {
        return Result<BuildResult, std::string>(
            std::string("CMake is not installed. Install it with: brew install cmake"));
    }

    auto install_dir = install_root_ / (package_name + "-" + version.to_string());
    auto build_dir_path = source_dir / ".drip-build";

    std::error_code ec;

    std::filesystem::create_directories(install_dir, ec);
    if (ec) {
        return Result<BuildResult, std::string>(
            std::string("Failed to create install directory: ") + ec.message());
    }

    auto config_cmd = "cmake -S " + source_dir.string() +
                      " -B " + build_dir_path.string() +
                      " -DCMAKE_INSTALL_PREFIX=" + install_dir.string() +
                      " -DCMAKE_BUILD_TYPE=Release 2>&1";
    auto config_out = exec(config_cmd);
    if (!std::filesystem::exists(build_dir_path / "CMakeCache.txt")) {
        return Result<BuildResult, std::string>(
            std::string("CMake configuration failed:\n") + config_out);
    }

    auto build_cmd = "cmake --build " + build_dir_path.string() +
                     " -j" + std::to_string(parallel_jobs()) + " 2>&1";
    auto build_out = exec(build_cmd);
    if (!std::filesystem::exists(build_dir_path / "build.ninja") &&
        !std::filesystem::exists(build_dir_path / "Makefile") &&
        !std::filesystem::exists(build_dir_path / "drip-build")) {
        auto any_marker = build_dir_path;
        bool has_valid_build = false;
        for (auto& entry : std::filesystem::directory_iterator(build_dir_path)) {
            if (entry.path().filename() == "CMakeCache.txt") continue;
            if (entry.is_regular_file() &&
                (entry.path().extension() == ".a" ||
                 entry.path().extension() == ".dylib" ||
                 entry.path().extension() == ".so")) {
                has_valid_build = true;
                break;
            }
        }
        if (!has_valid_build) {
            return Result<BuildResult, std::string>(
                std::string("CMake build failed:\n") + build_out);
        }
    }

    auto install_cmd = "cmake --install " + build_dir_path.string() +
                       " --prefix " + install_dir.string() + " 2>&1";
    auto install_out = exec(install_cmd);

    bool has_installed_files = false;
    if (std::filesystem::exists(install_dir)) {
        for (auto& entry : std::filesystem::recursive_directory_iterator(install_dir, ec)) {
            if (entry.is_regular_file()) {
                has_installed_files = true;
                break;
            }
        }
    }

    if (!has_installed_files) {
        return Result<BuildResult, std::string>(
            std::string("CMake install produced no files in ") + install_dir.string() +
            "\nInstall output:\n" + install_out);
    }

    return Result<BuildResult, std::string>(BuildResult{
        .install_path = install_dir
    });
}

}

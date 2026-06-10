#include "drip/cmd/install.hpp"
#include "drip/constants.hpp"
#include "drip/builder.hpp"
#include "drip/fetcher.hpp"
#include "drip/utils/git_utils.hpp"
#include "drip/utils/cmake_utils.hpp"
#include "drip/manifest.hpp"
#include "drip/resolver.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

int cmd_install() {
    auto manifest_result = drip::parse_manifest(MANIFEST);
    if (!manifest_result.has_value()) {
        std::cerr << "Error: " << manifest_result.error() << "\n";
        return 1;
    }

    auto& manifest = manifest_result.value();
    std::cout << "Installing dependencies for " << manifest.project_name << "\n";

    if (manifest.dependencies.empty()) {
        std::cout << "No dependencies\n";
        return 0;
    }

    if (!drip::CMakeBuilder::cmake_available()) {
        std::cerr << "CMake required. Install: brew install cmake\n";
        return 1;
    }

    drip::GitFetcher fetcher;
    drip::CMakeBuilder builder;
    std::vector<drip::ResolvedPackage> resolved;
    std::vector<std::pair<std::string, std::vector<std::string>>> dep_cmake_info;

    for (auto& dep : manifest.dependencies) {
        std::cout << "  " << dep.name << "...\n";

        auto source = drip::parse_git_ref(dep.git, dep.rev);
        auto fetch_result = fetcher.fetch(source);
        if (!fetch_result.has_value()) {
            std::cerr << "  Fetch failed: " << fetch_result.error() << "\n";
            return 1;
        }

        auto& fetched = fetch_result.value();
        auto version = drip::Version::parse(dep.rev.empty() ? FALLBACK_VERSION : dep.rev)
            .value_or(*drip::Version::parse(FALLBACK_VERSION));

        auto build_result = builder.build(fetched.source_path, dep.name, version);
        if (!build_result.has_value()) {
            std::cerr << "  Build failed: " << build_result.error() << "\n";
            return 1;
        }

        {
            auto install_dir = build_result.value().install_path;
            for (auto sub : CMAKE_SUBDIRS) {
                auto dir = install_dir / sub;
                std::error_code ec;
                if (!std::filesystem::exists(dir, ec)) continue;
                for (auto& entry : std::filesystem::directory_iterator(dir, ec)) {
                    if (!entry.is_directory()) continue;
                    auto pkg = entry.path().filename().string();
                    auto targets = parse_cmake_targets(entry.path());
                    if (!targets.empty()) {
                        dep_cmake_info.emplace_back(std::move(pkg), std::move(targets));
                    }
                }
            }
        }

        drip::ResolvedPackage rp;
        rp.name = dep.name;
        rp.version = version;
        rp.source_url = source.url;
        rp.content_hash = fetched.commit_hash;
        rp.install_path = build_result.value().install_path;
        resolved.push_back(std::move(rp));
    }

    auto lf = drip::Lockfile::from_resolution({.selected = resolved, .success = true});
    lf.write_to(LOCKFILE);

    {
        std::ofstream f(CMAKE_CONFIG);
        f << "set(DRIP_PACKAGE_DIR \"$ENV{HOME}" << DRIP_PACKAGES << "\")\n";
        f << "list(APPEND CMAKE_PREFIX_PATH\n";
        for (auto& rp : resolved)
            f << "    \"${DRIP_PACKAGE_DIR}" << rp.name << "-" << rp.version.to_string() << "\"\n";
        f << ")\n";
    }

    {
        std::ofstream f(CMAKE_DEPS);
        f << "set(DRIP_DEPS_TARGETS \"\")\n";
        for (auto& [pkg, targets] : dep_cmake_info) {
            f << "\nfind_package(" << pkg << " REQUIRED)\n";
            for (auto& t : targets) {
                f << "list(APPEND DRIP_DEPS_TARGETS " << t << ")\n";
            }
        }
    }

    std::cout << "Done. Run: drip build\n";
    return 0;
}

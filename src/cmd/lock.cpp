#include "drip/cmd/lock.hpp"
#include "drip/constants.hpp"
#include "drip/fetcher.hpp"
#include "drip/utils/git_utils.hpp"
#include "drip/manifest.hpp"
#include "drip/resolver.hpp"
#include <iostream>

int cmd_lock() {
    auto manifest_result = drip::parse_manifest(MANIFEST);
    if (!manifest_result.has_value()) {
        std::cerr << "Error: " << manifest_result.error() << "\n";
        return 1;
    }

    auto& manifest = manifest_result.value();
    std::cout << "Locking dependencies for " << manifest.project_name << "\n";

    if (manifest.dependencies.empty()) {
        std::cout << "No dependencies\n";
        return 0;
    }

    drip::GitFetcher fetcher;
    std::vector<drip::ResolvedPackage> resolved;

    for (auto& dep : manifest.dependencies) {
        std::cout << "  " << dep.name << "...\n";

        auto source = drip::parse_git_ref(dep.git, dep.rev);
        auto fetch_result = fetcher.fetch(source);
        if (!fetch_result.has_value()) {
            std::cerr << "  Failed: " << fetch_result.error() << "\n";
            return 1;
        }

        auto& fetched = fetch_result.value();
        auto version = drip::Version::parse(dep.rev.empty() ? FALLBACK_VERSION : dep.rev)
            .value_or(*drip::Version::parse(FALLBACK_VERSION));

        drip::ResolvedPackage rp;
        rp.name = dep.name;
        rp.version = version;
        rp.source_url = source.url;
        rp.content_hash = fetched.commit_hash;
        resolved.push_back(std::move(rp));
    }

    auto lf = drip::Lockfile::from_resolution({.selected = resolved, .success = true});
    lf.write_to(LOCKFILE);

    std::cout << "Locked " << manifest.dependencies.size()
              << " dep" << (manifest.dependencies.size() == 1 ? "" : "s")
              << " in " << LOCKFILE << "\n";
    return 0;
}

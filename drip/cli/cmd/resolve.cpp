#include <drip/cli/cmd/resolve.hpp>
#include <drip/core/resolver.hpp>
#include <iostream>

int cmd_resolve() {
    drip::Version v1 = *drip::Version::parse("10.2.0");
    drip::Version v2 = *drip::Version::parse("1.12.0");

    auto spdlog = std::move(*drip::Package::Builder().name("spdlog").version(v2).header_only(true).build());
    auto fmt1 = std::move(*drip::Package::Builder().name("fmt").version(v1)
        .dependency({.name = "spdlog", .version_constraint = ">=1.12.0"}).build());
    auto fmt2 = std::move(*drip::Package::Builder().name("fmt").version(v1)
        .dependency({.name = "spdlog", .version_constraint = ">=1.12.0"}).build());

    std::vector<drip::Package> pkgs;
    pkgs.push_back(std::move(spdlog));
    pkgs.push_back(std::move(fmt1));

    drip::DependencyResolver resolver(std::move(pkgs));
    auto result = resolver.resolve(fmt2, {"fmt>=10.0.0", "spdlog<1.14.0"});

    if (result.success) {
        std::cout << "Resolved " << result.selected.size() << " packages\n";
    } else {
        std::cout << "Conflict:\n";
        for (auto& c : result.conflicts) std::cout << "  " << c << "\n";
    }
    return 0;
}

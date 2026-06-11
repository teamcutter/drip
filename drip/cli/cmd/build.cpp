#include <drip/cli/cmd/build.hpp>
#include <drip/core/constants.hpp>
#include <drip/core/builder.hpp>
#include <drip/core/manifest.hpp>
#include <cstdlib>
#include <iostream>

int cmd_build() {
    if (!drip::CMakeBuilder::cmake_available()) {
        std::cerr << "CMake not found. Install: brew install cmake\n";
        return 1;
    }

    auto name = std::string(DEFAULT_PROJECT);
    {
        auto m = drip::parse_manifest(MANIFEST);
        if (m.has_value()) name = m.value().project_name;
    }

    if (std::system(CMAKE_CONFIGURE) != 0) {
        std::cerr << "Configure failed\n";
        return 1;
    }

    if (std::system(CMAKE_BUILD) != 0) {
        std::cerr << "Build failed\n";
        return 1;
    }

    std::cout << "Built " << name << "\n";
    return 0;
}

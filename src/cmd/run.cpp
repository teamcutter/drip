#include "drip/cmd/run.hpp"
#include "drip/cmd/build.hpp"
#include "drip/constants.hpp"
#include "drip/manifest.hpp"
#include <cstdlib>
#include <iostream>

int cmd_run() {
    auto name = std::string(DEFAULT_PROJECT);
    {
        auto m = drip::parse_manifest(MANIFEST);
        if (m.has_value()) name = m.value().project_name;
    }

    if (cmd_build() != 0) return 1;

    auto cmd = std::string(BINARY_PREFIX) + name;
    return std::system(cmd.c_str());
}

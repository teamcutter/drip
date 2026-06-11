#include <drip/cli/cmd/init.hpp>
#include <drip/core/constants.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

int cmd_init(int argc, char** argv) {
    auto name = (argc > 0) ? argv[0] : DEFAULT_PROJECT;

    std::error_code ec;
    std::filesystem::create_directories(PROJECT_DIR, ec);

    {
        std::ofstream f(MANIFEST);
        f << "[project]\n";
        f << "name = \"" << name << "\"\n";
        f << "version = \"" << DEFAULT_VERSION << "\"\n\n";
        f << DEPS_HEADER << "\n";
    }

    {
        std::ofstream f(SOURCE_TEMPLATE);
        f << "#include <iostream>\n\nint main() {\n";
        f << "    std::cout << \"Hello from " << name << "!\\n\";\n";
        f << "}\n";
    }

    {
        std::ofstream f(CMAKE_TEMPLATE);
        f << "cmake_minimum_required(VERSION 3.16)\n";
        f << "project(" << name << " LANGUAGES CXX)\n";
        f << "set(CMAKE_CXX_STANDARD 20)\n";
        f << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
        f << "include(" << CMAKE_CONFIG << ")\n";
        f << "include(" << CMAKE_DEPS << ")\n\n";
        f << "add_executable(" << name << " " << SOURCE_TEMPLATE << ")\n";
        f << "target_link_libraries(" << name << " PRIVATE ${DRIP_DEPS_TARGETS})\n";
    }

    std::cout << "Created " << name << "/" << MANIFEST << ", " << CMAKE_TEMPLATE << ", " << SOURCE_TEMPLATE << "\n";
    return 0;
}

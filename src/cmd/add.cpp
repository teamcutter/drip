#include "drip/cmd/add.hpp"
#include "drip/constants.hpp"
#include "drip/utils/git_utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

int cmd_add(int argc, char** argv) {
    if (argc < 1) {
        std::cerr << "Usage: " << argv[-1] << " add <gh:owner/repo|url>[@rev]\n";
        return 1;
    }

    auto input = std::string_view(argv[0]);
    auto name = drip::extract_pkg_name(input);

    auto git_part = input;
    auto at = git_part.find('@');
    if (at != std::string_view::npos) git_part = git_part.substr(0, at);

    std::string rev;
    auto at_pos = input.find('@');
    if (at_pos != std::string_view::npos) rev = std::string(input.substr(at_pos + 1));

    {
        std::string content;
        {
            std::ifstream in(MANIFEST);
            if (in) {
                std::stringstream ss;
                ss << in.rdbuf();
                content = ss.str();
            }
        }
        bool has_deps = content.find(DEPS_HEADER) != std::string::npos;

        std::ofstream f(MANIFEST, std::ios::app);
        if (!has_deps) f << "\n" << DEPS_HEADER << "\n";
        f << name << " = { git = \"" << std::string(git_part) << "\"";
        if (!rev.empty()) f << ", version = \"" << rev << "\"";
        f << " }\n";
    }

    std::cout << "Added " << name << "\n\n";
    return 0;
}

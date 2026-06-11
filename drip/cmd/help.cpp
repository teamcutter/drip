#include <drip/cmd/help.hpp>
#include <iostream>

void print_help(const char* prog) {
    std::cout << "drip - C++ package manager\n\n"
              << "Usage:\n"
              << "  " << prog << " init [name]                         Create new project\n"
              << "  " << prog << " add <url|gh:owner/repo>[@rev]      Add dep and install\n"
              << "  " << prog << " install                             Install deps\n"
              << "  " << prog << " build                               Build project\n"
              << "  " << prog << " run                                 Build and run\n"
              << "  " << prog << " lock                                Resolve deps to lockfile\n"
              << "  " << prog << " fetch <url> [rev]                   Fetch git repo\n";
}

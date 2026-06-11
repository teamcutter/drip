#include <drip/cli/cmd/help.hpp>
#include <drip/cli/cmd/init.hpp>
#include <drip/cli/cmd/add.hpp>
#include <drip/cli/cmd/fetch.hpp>
#include <drip/cli/cmd/build.hpp>
#include <drip/cli/cmd/run.hpp>
#include <drip/cli/cmd/install.hpp>
#include <drip/cli/cmd/resolve.hpp>
#include <drip/cli/cmd/lock.hpp>
#include <string_view>

int main(int argc, char** argv) {
    if (argc < 2) { print_help(argv[0]); return 0; }

    std::string_view cmd = argv[1];
    if (cmd == "init")    return cmd_init(argc - 2, argv + 2);
    if (cmd == "add")     return cmd_add(argc - 2, argv + 2);
    if (cmd == "fetch")   return cmd_fetch(argc - 2, argv + 2);
    if (cmd == "build")   return cmd_build();
    if (cmd == "run")     return cmd_run();
    if (cmd == "install") return cmd_install();
    if (cmd == "resolve") return cmd_resolve();
    if (cmd == "lock")    return cmd_lock();

    print_help(argv[0]);
    return 1;
}

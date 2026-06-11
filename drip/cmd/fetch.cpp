#include <drip/cmd/fetch.hpp>
#include <drip/constants.hpp>
#include <drip/fetcher.hpp>
#include <drip/details/git_utils.hpp>
#include <iostream>

int cmd_fetch(int argc, char** argv) {
    if (argc < 1) {
        std::cerr << "Usage: " << argv[-1] << " fetch <url|gh:owner/repo> [rev]\n";
        return 1;
    }

    auto source = drip::parse_git_ref(argv[0]);
    if (argc > 1) source.rev = argv[1];

    std::cout << "Fetching " << source.url;
    if (!source.rev.empty()) std::cout << " @ " << source.rev;
    std::cout << "...\n";

    drip::GitFetcher fetcher;
    auto result = fetcher.fetch(source);

    if (!result.has_value()) {
        std::cerr << "Error: " << result.error() << "\n";
        return 1;
    }

    auto& fetched = result.value();
    std::cout << "Fetched to: " << fetched.source_path << "\n";
    std::cout << "Commit:     " << fetched.commit_hash << "\n";
    return 0;
}

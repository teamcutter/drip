#include <drip/details/git_utils.hpp>
#include <drip/core/constants.hpp>

namespace drip {

GitSource parse_git_ref(std::string_view input, std::string_view rev) {
    if (input.starts_with(GH_PREFIX)) {
        input = input.substr(3);
        auto at = input.find('@');
        if (at != std::string_view::npos) {
            if (rev.empty()) rev = input.substr(at + 1);
            return {.url = std::string(GITHUB_URL) + std::string(input.substr(0, at)) + GIT_SUFFIX, .rev = std::string(rev)};
        }
        return {.url = std::string(GITHUB_URL) + std::string(input) + GIT_SUFFIX, .rev = std::string(rev)};
    }
    if (input.starts_with(GITHUB_PREFIX)) {
        input = input.substr(7);
        auto at = input.find('@');
        if (at != std::string_view::npos) {
            if (rev.empty()) rev = input.substr(at + 1);
            return {.url = std::string(GITHUB_URL) + std::string(input.substr(0, at)) + GIT_SUFFIX, .rev = std::string(rev)};
        }
        return {.url = std::string(GITHUB_URL) + std::string(input) + GIT_SUFFIX, .rev = std::string(rev)};
    }
    auto at = input.find('@');
    if (at != std::string_view::npos) {
        if (rev.empty()) rev = input.substr(at + 1);
        return {.url = std::string(input.substr(0, at)), .rev = std::string(rev)};
    }
    return {.url = std::string(input), .rev = std::string(rev)};
}

std::string extract_pkg_name(std::string_view input) {
    if (input.starts_with(GH_PREFIX))
        input = input.substr(3);
    else if (input.starts_with(GITHUB_PREFIX))
        input = input.substr(7);
    auto at = input.find('@');
    if (at != std::string_view::npos) input = input.substr(0, at);
    auto slash = input.rfind('/');
    if (slash != std::string_view::npos) input = input.substr(slash + 1);
    if (input.ends_with(GIT_SUFFIX)) input = input.substr(0, input.size() - 4);
    return std::string(input);
}

}

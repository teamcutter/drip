#pragma once

#include "drip/fetcher.hpp"
#include <cstdlib>
#include <sstream>
#include <array>
#include <memory>
#include <functional>

namespace drip {

namespace {

std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, std::function<void(FILE*)>> pipe(
        popen(cmd.c_str(), "r"),
        [](FILE* f) { pclose(f); });
    if (!pipe) return {};
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    while (!result.empty() && (result.back() == '\n' || result.back() == ' ')) {
        result.pop_back();
    }
    return result;
}

std::string sha256_hex(const std::string& input) {
    std::string cmd = "echo -n \"" + input + "\" | shasum -a 256 | cut -d' ' -f1";
    return exec(cmd);
}

}

GitFetcher::GitFetcher()
    : cache_dir_(default_cache_dir()) {}

GitFetcher::GitFetcher(std::filesystem::path cache_dir)
    : cache_dir_(std::move(cache_dir)) {}

std::filesystem::path GitFetcher::default_cache_dir() {
    auto home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".drip" / "cache";
    }
    return std::filesystem::path("/tmp") / "drip-cache";
}

void GitFetcher::set_cache_dir(std::filesystem::path dir) {
    cache_dir_ = std::move(dir);
}

std::string GitFetcher::cache_key(const std::string& url) const {
    return sha256_hex(url);
}

bool GitFetcher::is_cached(const std::string& key) const {
    return std::filesystem::exists(cache_dir_ / key / ".git");
}

std::string GitFetcher::current_commit(const std::filesystem::path& repo) const {
    return exec("git -C " + repo.string() + " rev-parse --short HEAD 2>/dev/null");
}

Result<FetchResult, std::string> GitFetcher::fetch(const GitSource& source) {
    auto key = cache_key(source.url);
    auto target = cache_dir_ / key;

    std::error_code ec;
    std::filesystem::create_directories(cache_dir_, ec);
    if (ec) {
        return Result<FetchResult, std::string>(
            std::string("Failed to create cache directory: ") + ec.message());
    }

    if (!is_cached(key)) {
        std::string clone_cmd = "git clone --depth 1 " + source.url + " " + target.string() + " 2>&1";
        auto clone_out = exec(clone_cmd);
        if (!std::filesystem::exists(target / ".git")) {
            return Result<FetchResult, std::string>(
                std::string("Git clone failed: ") + clone_out);
        }
    }

    if (!source.rev.empty()) {
        auto checkout_cmd = "git -C " + target.string() + " checkout " + source.rev + " 2>&1";
        auto checkout_out = exec(checkout_cmd);
        auto hash = current_commit(target);
        if (hash.empty()) {
            return Result<FetchResult, std::string>(
                std::string("Failed to checkout revision: ") + checkout_out);
        }
    }

    auto hash = current_commit(target);
    if (hash.empty()) {
        return Result<FetchResult, std::string>("Failed to get commit hash after fetch");
    }

    return Result<FetchResult, std::string>(FetchResult{
        .source_path = target,
        .commit_hash = std::move(hash)
    });
}

}

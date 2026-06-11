#pragma once

#include <drip/result.hpp>
#include <drip/package.hpp>
#include <filesystem>
#include <string>

namespace drip {

struct FetchResult {
    std::filesystem::path source_path;
    std::string commit_hash;
};

class GitFetcher {
public:
    GitFetcher();
    explicit GitFetcher(std::filesystem::path cache_dir);

    Result<FetchResult, std::string> fetch(const GitSource& source);

    static std::filesystem::path default_cache_dir();
    void set_cache_dir(std::filesystem::path dir);

private:
    bool is_cached(const std::string& key) const;
    std::string cache_key(const std::string& url) const;
    std::string current_commit(const std::filesystem::path& repo) const;

    std::filesystem::path cache_dir_;
};

}

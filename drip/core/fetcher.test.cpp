#include <gtest/gtest.h>
#include <drip/core/fetcher.hpp>
#include <cstdlib>

using namespace drip;

TEST(FetcherTest, DefaultCacheDirWithHome) {
    auto home = std::getenv("HOME");
    if (home) {
        auto dir = GitFetcher::default_cache_dir();
        EXPECT_EQ(dir, std::filesystem::path(home) / ".drip" / "cache");
    }
}

TEST(FetcherTest, DefaultCacheDirNonEmpty) {
    auto dir = GitFetcher::default_cache_dir();
    EXPECT_FALSE(dir.empty());
}

TEST(FetcherTest, ParameterizedConstructor) {
    std::filesystem::path custom("/tmp/drip-test-cache");
    GitFetcher fetcher(custom);
    SUCCEED();
}

TEST(FetcherTest, SetCacheDirDoesNotAffectStaticDefault) {
    GitFetcher fetcher;
    auto static_default = GitFetcher::default_cache_dir();
    fetcher.set_cache_dir("/some/custom/path");
    EXPECT_EQ(GitFetcher::default_cache_dir(), static_default);
}

TEST(FetcherTest, FetchWithEmptyUrlReturnsError) {
    GitFetcher fetcher;
    GitSource src{"", ""};
    auto result = fetcher.fetch(src);
    EXPECT_FALSE(result.has_value());
}

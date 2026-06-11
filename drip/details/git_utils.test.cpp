#include <gtest/gtest.h>
#include <drip/details/git_utils.hpp>

using namespace drip;

TEST(GitUtilsTest, ParseGitRefGhSimple) {
    auto r = parse_git_ref("gh:fmtlib/fmt");
    EXPECT_EQ(r.url, "https://github.com/fmtlib/fmt.git");
    EXPECT_TRUE(r.rev.empty());
}

TEST(GitUtilsTest, ParseGitRefGhWithRev) {
    auto r = parse_git_ref("gh:fmtlib/fmt@10.2.0");
    EXPECT_EQ(r.url, "https://github.com/fmtlib/fmt.git");
    EXPECT_EQ(r.rev, "10.2.0");
}

TEST(GitUtilsTest, ParseGitRefGithubPrefix) {
    auto r = parse_git_ref("github:google/googletest");
    EXPECT_EQ(r.url, "https://github.com/google/googletest.git");
}

TEST(GitUtilsTest, ParseGitRefFullUrl) {
    auto r = parse_git_ref("https://github.com/user/repo.git");
    EXPECT_EQ(r.url, "https://github.com/user/repo.git");
    EXPECT_TRUE(r.rev.empty());
}

TEST(GitUtilsTest, ParseGitRefFullUrlWithRev) {
    auto r = parse_git_ref("https://example.com/lib.git@v1.0");
    EXPECT_EQ(r.url, "https://example.com/lib.git");
    EXPECT_EQ(r.rev, "v1.0");
}

TEST(GitUtilsTest, ParseGitRefOverrideRev) {
    auto r = parse_git_ref("gh:user/repo@from_arg", "explicit");
    EXPECT_EQ(r.url, "https://github.com/user/repo.git");
    EXPECT_EQ(r.rev, "explicit");
}

TEST(GitUtilsTest, ExtractPkgNameGh) {
    EXPECT_EQ(extract_pkg_name("gh:fmtlib/fmt"), "fmt");
}

TEST(GitUtilsTest, ExtractPkgNameGithub) {
    EXPECT_EQ(extract_pkg_name("github:google/googletest"), "googletest");
}

TEST(GitUtilsTest, ExtractPkgNameWithRev) {
    EXPECT_EQ(extract_pkg_name("gh:user/repo@v1.0"), "repo");
}

TEST(GitUtilsTest, ExtractPkgNameFullUrl) {
    EXPECT_EQ(extract_pkg_name("https://github.com/user/my-lib.git"), "my-lib");
}

TEST(GitUtilsTest, ExtractPkgNameNoSlash) {
    EXPECT_EQ(extract_pkg_name("gh:my-pkg"), "my-pkg");
}

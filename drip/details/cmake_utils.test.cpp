#include <gtest/gtest.h>
#include <drip/details/cmake_utils.hpp>
#include <algorithm>
#include <filesystem>
#include <fstream>

TEST(CMakeUtilsTest, ToLowerEmpty) {
    EXPECT_EQ(to_lower(""), "");
}

TEST(CMakeUtilsTest, ToLowerAlreadyLower) {
    EXPECT_EQ(to_lower("abc"), "abc");
}

TEST(CMakeUtilsTest, ToLowerMixedCase) {
    EXPECT_EQ(to_lower("AbC"), "abc");
}

TEST(CMakeUtilsTest, ToLowerAllUpper) {
    EXPECT_EQ(to_lower("HELLO"), "hello");
}

TEST(CMakeUtilsTest, ToLowerWithNumbers) {
    EXPECT_EQ(to_lower("Foo123Bar"), "foo123bar");
}

TEST(CMakeUtilsTest, ParseTargetsEmptyDir) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_empty";
    std::filesystem::create_directories(tmp);
    auto targets = parse_cmake_targets(tmp);
    EXPECT_TRUE(targets.empty());
    std::filesystem::remove_all(tmp);
}

TEST(CMakeUtilsTest, ParseTargetsSkipsNonTargetsFiles) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_skip";
    std::filesystem::create_directories(tmp);
    std::ofstream(tmp / "README.md") << "add_library(foo)";
    auto targets = parse_cmake_targets(tmp);
    EXPECT_TRUE(targets.empty());
    std::filesystem::remove_all(tmp);
}

TEST(CMakeUtilsTest, ParseTargetsBasic) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_basic";
    std::filesystem::create_directories(tmp);
    {
        std::ofstream f(tmp / "fooTargets.cmake");
        f << "add_library(foo STATIC IMPORTED)\n";
    }
    auto targets = parse_cmake_targets(tmp);
    ASSERT_EQ(targets.size(), 1u);
    EXPECT_EQ(targets[0], "foo");
    std::filesystem::remove_all(tmp);
}

TEST(CMakeUtilsTest, ParseTargetsSkipsVariantSuffixes) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_variants";
    std::filesystem::create_directories(tmp);
    {
        std::ofstream f(tmp / "fmtTargets.cmake");
        f << "add_library(fmt STATIC IMPORTED)\n";
        f << "add_library(fmt-main STATIC IMPORTED)\n";
        f << "add_library(fmt-header-only INTERFACE IMPORTED)\n";
        f << "add_library(fmt-static STATIC IMPORTED)\n";
        f << "add_library(fmt-shared SHARED IMPORTED)\n";
    }
    auto targets = parse_cmake_targets(tmp);
    ASSERT_EQ(targets.size(), 1u);
    EXPECT_EQ(targets[0], "fmt");
    std::filesystem::remove_all(tmp);
}

TEST(CMakeUtilsTest, ParseTargetsSkipsTargetsDashPattern) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_dash";
    std::filesystem::create_directories(tmp);
    {
        std::ofstream f(tmp / "fmtTargets-detail.cmake");
        f << "add_library(detail_stuff STATIC IMPORTED)\n";
    }
    auto targets = parse_cmake_targets(tmp);
    EXPECT_TRUE(targets.empty());
    std::filesystem::remove_all(tmp);
}

TEST(CMakeUtilsTest, ParseTargetsMultipleFiles) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_multi";
    std::filesystem::create_directories(tmp);
    {
        std::ofstream f(tmp / "fmtTargets.cmake");
        f << "add_library(fmt STATIC IMPORTED)\n";
    }
    {
        std::ofstream f(tmp / "spdlogTargets.cmake");
        f << "add_library(spdlog SHARED IMPORTED)\n";
    }
    auto targets = parse_cmake_targets(tmp);
    EXPECT_EQ(targets.size(), 2u);
    EXPECT_NE(std::find(targets.begin(), targets.end(), "fmt"), targets.end());
    EXPECT_NE(std::find(targets.begin(), targets.end(), "spdlog"), targets.end());
    std::filesystem::remove_all(tmp);
}

TEST(CMakeUtilsTest, ParseTargetsCaseInsensitiveFilename) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_case";
    std::filesystem::create_directories(tmp);
    {
        std::ofstream f(tmp / "FOOTARGETS.cmake");
        f << "add_library(foo STATIC IMPORTED)\n";
    }
    auto targets = parse_cmake_targets(tmp);
    ASSERT_EQ(targets.size(), 1u);
    EXPECT_EQ(targets[0], "foo");
    std::filesystem::remove_all(tmp);
}

TEST(CMakeUtilsTest, ParseTargetsDeduplicates) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_cmake_dedup";
    std::filesystem::create_directories(tmp);
    {
        std::ofstream f(tmp / "fooTargets.cmake");
        f << "add_library(foo STATIC IMPORTED)\n";
        f << "add_library(foo SHARED IMPORTED)\n";
    }
    auto targets = parse_cmake_targets(tmp);
    ASSERT_EQ(targets.size(), 1u);
    EXPECT_EQ(targets[0], "foo");
    std::filesystem::remove_all(tmp);
}

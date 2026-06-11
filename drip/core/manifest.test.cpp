#include <gtest/gtest.h>
#include <drip/core/manifest.hpp>
#include <fstream>

using namespace drip;

TEST(ManifestTest, ParseBasic) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_test_manifest.toml";
    std::ofstream(tmp) << "[project]\nname = \"testproj\"\nversion = \"0.1.0\"\n\n[dependencies]\n";
    auto m = parse_manifest(tmp);
    ASSERT_TRUE(m.has_value());
    EXPECT_EQ(m.value().project_name, "testproj");
    EXPECT_EQ(m.value().project_version, "0.1.0");
    std::filesystem::remove(tmp);
}

TEST(ManifestTest, ParseWithDeps) {
    auto tmp = std::filesystem::temp_directory_path() / "drip_test_deps.toml";
    std::ofstream(tmp) << "[project]\nname = \"myapp\"\nversion = \"1.0.0\"\n\n[dependencies]\n"
                          "fmt = { git = \"gh:fmtlib/fmt\", version = \"10.2.0\" }\n"
                          "gtest = { git = \"gh:google/googletest\", version = \"1.15.2\" }\n";
    auto m = parse_manifest(tmp);
    ASSERT_TRUE(m.has_value());
    EXPECT_EQ(m.value().project_name, "myapp");
    EXPECT_EQ(m.value().dependencies.size(), 2u);
    EXPECT_EQ(m.value().dependencies[0].name, "fmt");
    EXPECT_EQ(m.value().dependencies[0].git, "gh:fmtlib/fmt");
    EXPECT_EQ(m.value().dependencies[0].rev, "10.2.0");
    EXPECT_EQ(m.value().dependencies[1].name, "gtest");
    std::filesystem::remove(tmp);
}

TEST(ManifestTest, ParseMissingFile) {
    auto m = parse_manifest("/nonexistent/path.toml");
    EXPECT_FALSE(m.has_value());
}

TEST(ManifestTest, CreateTemplate) {
    auto m = create_template_manifest("hello");
    EXPECT_EQ(m.project_name, "hello");
    EXPECT_EQ(m.dependencies.size(), 0u);
}

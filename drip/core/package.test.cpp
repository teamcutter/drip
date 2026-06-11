#include <gtest/gtest.h>
#include <drip/core/package.hpp>

using namespace drip;

TEST(PackageTest, BuilderRequiresName) {
    auto result = Package::Builder().build();
    EXPECT_FALSE(result.has_value());
}

TEST(PackageTest, BuilderWithName) {
    auto v = Version::parse("1.0.0");
    ASSERT_TRUE(v.has_value());
    auto result = Package::Builder()
        .name("testpkg")
        .version(*v)
        .build();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().name(), "testpkg");
    EXPECT_EQ(result.value().version().to_string(), "1.0.0");
}

TEST(PackageTest, BuilderWithDeps) {
    auto v = Version::parse("2.0.0");
    ASSERT_TRUE(v.has_value());
    Dependency dep{"fmt", "10.2.0", false, std::nullopt};
    auto result = Package::Builder()
        .name("app")
        .version(*v)
        .dependency(std::move(dep))
        .build();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().name(), "app");
    ASSERT_EQ(result.value().dependencies().size(), 1u);
    EXPECT_EQ(result.value().dependencies()[0].name, "fmt");
    EXPECT_EQ(result.value().dependencies()[0].version_constraint, "10.2.0");
}

TEST(PackageTest, BuilderMetadata) {
    auto v = Version::parse("0.5.0");
    ASSERT_TRUE(v.has_value());
    auto result = Package::Builder()
        .name("lib")
        .version(*v)
        .metadata("author", "me")
        .build();
    ASSERT_TRUE(result.has_value());
    auto& meta = result.value().metadata();
    auto it = meta.find("author");
    ASSERT_NE(it, meta.end());
    EXPECT_EQ(it->second, "me");
}

TEST(PackageTest, PackageDefaultConstructor) {
    Package pkg;
    EXPECT_TRUE(pkg.name().empty());
}

TEST(PackageTest, PackageNameVersionConstructor) {
    auto v = Version::parse("3.1.4");
    ASSERT_TRUE(v.has_value());
    Package pkg("mypkg", *v);
    EXPECT_EQ(pkg.name(), "mypkg");
    EXPECT_EQ(pkg.version().to_string(), "3.1.4");
}

TEST(PackageTest, ResolvedPackageToTomlMinimal) {
    auto v = Version::parse("1.0.0");
    ASSERT_TRUE(v.has_value());
    ResolvedPackage rp;
    rp.name = "test";
    rp.version = *v;
    auto toml = rp.to_toml();
    EXPECT_NE(toml.find("name = \"test\""), std::string::npos);
    EXPECT_NE(toml.find("version = \"1.0.0\""), std::string::npos);
    EXPECT_EQ(toml.find("dependencies = ["), std::string::npos);
}

TEST(PackageTest, ResolvedPackageToTomlFull) {
    auto v = Version::parse("10.2.0");
    ASSERT_TRUE(v.has_value());
    ResolvedPackage rp;
    rp.name = "fmt";
    rp.version = *v;
    rp.source_url = "https://github.com/fmtlib/fmt.git";
    rp.content_hash = "abc123";
    rp.dependencies = {"cmake"};
    rp.install_path = "/tmp/fmt";
    auto toml = rp.to_toml();
    EXPECT_NE(toml.find("name = \"fmt\""), std::string::npos);
    EXPECT_NE(toml.find("version = \"10.2.0\""), std::string::npos);
    EXPECT_NE(toml.find("source_url = \"https://github.com/fmtlib/fmt.git\""), std::string::npos);
    EXPECT_NE(toml.find("content_hash = \"abc123\""), std::string::npos);
    EXPECT_NE(toml.find("dependencies = [\"cmake\"]"), std::string::npos);
    EXPECT_NE(toml.find("install_path = \"/tmp/fmt\""), std::string::npos);
}

TEST(PackageTest, ResolvedPackageFromToml) {
    auto rp = ResolvedPackage::from_toml("name = \"x\"\nversion = \"1.0.0\"\n");
    EXPECT_FALSE(rp.has_value());
}

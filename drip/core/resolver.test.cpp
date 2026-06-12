#include <gtest/gtest.h>
#include <drip/core/resolver.hpp>

using namespace drip;

TEST(ResolverTest, LockfileFromResolutionEmpty) {
    ResolutionResult rr;
    rr.success = true;
    auto lf = Lockfile::from_resolution(rr);
    EXPECT_EQ(lf.to_toml(), "");
}

TEST(ResolverTest, LockfileToTomlSingle) {
    auto v = Version::parse("1.0.0");
    ASSERT_TRUE(v.has_value());
    ResolvedPackage rp;
    rp.name = "fmt";
    rp.version = *v;
    ResolutionResult rr;
    rr.success = true;
    rr.selected.push_back(rp);
    auto lf = Lockfile::from_resolution(rr);
    auto toml = lf.to_toml();
    EXPECT_NE(toml.find("[[packages]]"), std::string::npos);
    EXPECT_NE(toml.find("name = \"fmt\""), std::string::npos);
    EXPECT_NE(toml.find("version = \"1.0.0\""), std::string::npos);
}

TEST(ResolverTest, LockfileToTomlMultiple) {
    auto v1 = Version::parse("1.0.0");
    auto v2 = Version::parse("2.0.0");
    ASSERT_TRUE(v1.has_value());
    ASSERT_TRUE(v2.has_value());
    ResolvedPackage a, b;
    a.name = "a"; a.version = *v1;
    b.name = "b"; b.version = *v2;
    ResolutionResult rr;
    rr.success = true;
    rr.selected = {a, b};
    auto lf = Lockfile::from_resolution(rr);
    auto toml = lf.to_toml();
    auto count = 0;
    for (size_t pos = 0; (pos = toml.find("[[packages]]", pos)) != std::string::npos; pos++)
        count++;
    EXPECT_EQ(count, 2);
}

TEST(ResolverTest, ResolverConstructorGroupsByName) {
    auto v1 = Version::parse("1.0.0");
    auto v2 = Version::parse("2.0.0");
    ASSERT_TRUE(v1.has_value());
    ASSERT_TRUE(v2.has_value());
    std::vector<Package> pkgs;
    pkgs.emplace_back("fmt", *v1);
    pkgs.emplace_back("fmt", *v2);
    DependencyResolver resolver(std::move(pkgs));
    SUCCEED();
}

TEST(ResolverTest, ResolveSimpleNoDeps) {
    auto v = Version::parse("1.0.0");
    ASSERT_TRUE(v.has_value());
    Package root("root", *v);
    DependencyResolver resolver({});
    auto result = resolver.resolve(root);
    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.selected.size(), 1u);
    EXPECT_EQ(result.selected[0].name, "root");
}

TEST(ResolverTest, ResolveWithConstraint) {
    auto v = Version::parse("1.0.0");
    ASSERT_TRUE(v.has_value());
    Package root("root", *v);
    DependencyResolver resolver({});
    auto result = resolver.resolve(root, {});
    EXPECT_TRUE(result.success);
}

TEST(ResolverTest, ResolveWithDependency) {
    auto v1 = Version::parse("1.0.0");
    auto v2 = Version::parse("2.0.0");
    ASSERT_TRUE(v1.has_value());
    ASSERT_TRUE(v2.has_value());

    Dependency dep{"b", ">=2.0", false, std::nullopt};
    auto builder = Package::Builder()
        .name("a")
        .version(*v1)
        .dependency(std::move(dep));
    auto root = builder.build();
    ASSERT_TRUE(root.has_value());

    std::vector<Package> dep_pkgs;
    dep_pkgs.emplace_back("b", *v2);
    DependencyResolver resolver(std::move(dep_pkgs));
    auto result = resolver.resolve(root.value());
    EXPECT_TRUE(result.success);
    bool found_b = false;
    for (auto& pkg : result.selected) {
        if (pkg.name == "b") found_b = true;
    }
    EXPECT_TRUE(found_b);
}

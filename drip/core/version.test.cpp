#include <gtest/gtest.h>
#include <drip/core/version.hpp>

using namespace drip;

TEST(VersionTest, ParseSimple) {
    auto v = Version::parse("1.2.3");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->to_string(), "1.2.3");
}

TEST(VersionTest, ParseStripsLeadingV) {
    auto v = Version::parse("v3.11.3");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->to_string(), "3.11.3");
}

TEST(VersionTest, ParseWithPrerelease) {
    auto v = Version::parse("2.0.0-beta1");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->to_string(), "2.0.0-beta1");
}

TEST(VersionTest, ParseWithBuild) {
    auto v = Version::parse("1.0.0+sha.abc123");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->to_string(), "1.0.0+sha.abc123");
}

TEST(VersionTest, ParseInvalid) {
    EXPECT_FALSE(Version::parse("abc").has_value());
    EXPECT_FALSE(Version::parse("").has_value());
}

TEST(VersionTest, Comparison) {
    auto v1 = Version::parse("1.0.0");
    auto v2 = Version::parse("2.0.0");
    ASSERT_TRUE(v1.has_value());
    ASSERT_TRUE(v2.has_value());
    EXPECT_TRUE(*v1 < *v2);
    EXPECT_TRUE(*v2 > *v1);
    EXPECT_TRUE(*v1 == *v1);
}

TEST(VersionTest, ToString) {
    auto v = Version::parse("10.2.0");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->to_string(), "10.2.0");
}

#include <gtest/gtest.h>
#include <drip/core/result.hpp>
#include <string>

using namespace drip;

TEST(ResultTest, ValueConstruction) {
    Result<int> r(42);
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 42);
    EXPECT_EQ(*r, 42);
}

TEST(ResultTest, ErrorConstruction) {
    Result<int> r(Error{"fail"});
    EXPECT_FALSE(r.has_value());
    EXPECT_EQ(r.error().get_message(), "fail");
}

TEST(ResultTest, StringValue) {
    Result<std::string> r(std::string("hello"));
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), "hello");
    EXPECT_EQ(*r, "hello");
}

TEST(ResultTest, StringError) {
    Result<std::string> r(Error{"oops"});
    EXPECT_FALSE(r.has_value());
    EXPECT_EQ(r.error().get_message(), "oops");
}

TEST(ResultTest, MoveValue) {
    Result<std::string> r(std::string("move me"));
    EXPECT_TRUE(r.has_value());
    auto moved = std::move(*r);
    EXPECT_EQ(moved, "move me");
}

TEST(ResultTest, ConstAccess) {
    const Result<int> r(100);
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 100);
    EXPECT_EQ(*r, 100);
}

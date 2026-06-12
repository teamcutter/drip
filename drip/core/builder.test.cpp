#include <gtest/gtest.h>
#include <drip/core/builder.hpp>
#include <cstdlib>

using namespace drip;

TEST(BuilderTest, DefaultInstallRootWithHome) {
    auto home = std::getenv("HOME");
    if (home) {
        auto dir = CMakeBuilder::default_install_root();
        EXPECT_EQ(dir, std::filesystem::path(home) / ".drip" / "packages");
    }
}

TEST(BuilderTest, DefaultInstallRootNonEmpty) {
    auto dir = CMakeBuilder::default_install_root();
    EXPECT_FALSE(dir.empty());
}

TEST(BuilderTest, ParallelJobsDefault) {
    CMakeBuilder builder;
    EXPECT_GT(builder.parallel_jobs(), 0);
}

TEST(BuilderTest, SetParallelJobs) {
    CMakeBuilder builder;
    builder.set_parallel_jobs(4);
    EXPECT_EQ(builder.parallel_jobs(), 4);
}

TEST(BuilderTest, SetParallelJobsZeroUsesDefault) {
    CMakeBuilder builder;
    builder.set_parallel_jobs(0);
    EXPECT_GT(builder.parallel_jobs(), 0);
}

TEST(BuilderTest, CmakeAvailable) {
    EXPECT_TRUE(CMakeBuilder::cmake_available());
}

TEST(BuilderTest, ParameterizedConstructor) {
    CMakeBuilder builder("/tmp/drip-install");
    SUCCEED();
}

TEST(BuilderTest, SetInstallRoot) {
    CMakeBuilder builder;
    builder.set_install_root("/custom/path");
    SUCCEED();
}

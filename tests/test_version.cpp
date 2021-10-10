#include <gtest/gtest.h>

#include <hello.h>

TEST(version, is_version_valid) { ASSERT_TRUE(version::version() > 0); }

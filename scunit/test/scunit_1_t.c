#include "scunit/scunit.h"

TEST(First)
{
    EXPECT_TRUE(1);
    EXPECT_FALSE(1);
    EXPECT_FALSE(0);
    EXPECT_TRUE(0);
    EXPECT_EQ(1, 2);
    EXPECT_EQ(1, 1);
    EXPECT_NE(2, 1 + 1);
    EXPECT_NE(1, 1);
    ASSERT_EQ(1, 1);
    ASSERT_EQ(1, 1 + 1);
    EXPECT_EQ(1, 2);
}

TEST(Second)
{
    ASSERT_EQ(1, 1);
}

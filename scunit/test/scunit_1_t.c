#include "scunit/scunit.h"
#include <unistd.h>

TEST(First)
{
    sleep(1);
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
    sleep(2);
    ASSERT_EQ(1, 1);
}

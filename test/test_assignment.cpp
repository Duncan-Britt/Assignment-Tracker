#include <gtest/gtest.h>
#include "../src/assignment.h"

TEST(TestSuite, TestName)
{
    ASSERT_EQ(5, 5);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

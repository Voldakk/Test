#include "EVA/Test/Test.hpp"
#include <iostream>

int main()
{
    RUN_ALL_TESTS();
    return 0;
}

TEST(ShouldPass, All)
{
    struct S
    {
        int i;
        float f;
    };

    S a = { 1, 3.0f };
    S b = { 1, 3.0f };
    S c = { 1, 2.0f };


    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
    EXPECT_EQ(2, 2);
    EXPECT_NE(2, 3);

    EXPECT_LT(2, 3);
    EXPECT_LE(2, 2);
    EXPECT_LE(2, 3);

    EXPECT_GT(3, 2);
    EXPECT_GE(3, 2);
    EXPECT_GE(3, 3);

    EXPECT_STREQ("abc", "abc");
    EXPECT_STRNQ("abc", "cba");

    EXPECT_MEMEQ(&a, &b, sizeof(S));
    EXPECT_MEMNQ(&a, &c, sizeof(S));

    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
    ASSERT_EQ(2, 2);
    ASSERT_NE(2, 3);

    ASSERT_LT(2, 3);
    ASSERT_LE(2, 2);
    ASSERT_LE(2, 3);

    ASSERT_GT(3, 2);
    ASSERT_GE(3, 2);
    ASSERT_GE(3, 3);

    ASSERT_STREQ("abc", "abc");
    ASSERT_STRNQ("abc", "cba");

    ASSERT_MEMEQ(&a, &b, sizeof(S));
    ASSERT_MEMNQ(&a, &c, sizeof(S));
}


TEST(ShouldFail, ET) { EXPECT_TRUE(false); }
TEST(ShouldFail, EF) { EXPECT_FALSE(true); }

TEST(ShouldFail, EEQ) { EXPECT_EQ(2, 3); }
TEST(ShouldFail, ENQ) { EXPECT_NE(2, 2); }

TEST(ShouldFail, ELT) { EXPECT_LT(3, 3); }
TEST(ShouldFail, ELT2) { EXPECT_LT(3, 2); }
TEST(ShouldFail, ELE) { EXPECT_LE(3, 2); }

TEST(ShouldFail, EGT) { EXPECT_GT(3, 3); }
TEST(ShouldFail, EGT2) { EXPECT_GT(2, 3); }
TEST(ShouldFail, EGE) { EXPECT_GE(2, 3); }

TEST(ShouldFail, ESTREQ) { EXPECT_STREQ("abc", "cba"); }
TEST(ShouldFail, ESTRNQ) { EXPECT_STRNQ("abc", "abc"); }

TEST(ShouldFail, EMEMEQ)
{
    struct S
    {
        int i;
        float f;
    };
    S a = { 1, 3.0f };
    S c = { 1, 2.0f };
    EXPECT_MEMEQ(&a, &c, sizeof(S));
}
TEST(ShouldFail, EMEMNQ)
{
    struct S
    {
        int i;
        float f;
    };
    S a = { 1, 3.0f };
    S b = { 1, 3.0f };
    EXPECT_MEMNQ(&a, &b, sizeof(S));
}


TEST(ShouldFail, AT) { ASSERT_TRUE(false); }
TEST(ShouldFail, AF) { ASSERT_FALSE(true); }

TEST(ShouldFail, AEQ) { ASSERT_EQ(2, 3); }
TEST(ShouldFail, ANQ) { ASSERT_NE(2, 2); }

TEST(ShouldFail, ALT) { ASSERT_LT(3, 3); }
TEST(ShouldFail, ALT2) { ASSERT_LT(3, 2); }
TEST(ShouldFail, ALE) { ASSERT_LE(3, 2); }

TEST(ShouldFail, AGT) { ASSERT_GT(3, 3); }
TEST(ShouldFail, AGT2) { ASSERT_GT(2, 3); }
TEST(ShouldFail, AGE) { ASSERT_GE(2, 3); }

TEST(ShouldFail, ASTREQ) { ASSERT_STREQ("abc", "cba"); }
TEST(ShouldFail, ASTRNQ) { ASSERT_STRNQ("abc", "abc"); }

TEST(ShouldFail, AMEMEQ)
{
    struct S
    {
        int i;
        float f;
    };
    S a = { 1, 3.0f };
    S c = { 1, 2.0f };
    ASSERT_MEMEQ(&a, &c, sizeof(S));
}
TEST(ShouldFail, AMEMNQ)
{
    struct S
    {
        int i;
        float f;
    };
    S a = { 1, 3.0f };
    S b = { 1, 3.0f };
    ASSERT_MEMNQ(&a, &b, sizeof(S));
}

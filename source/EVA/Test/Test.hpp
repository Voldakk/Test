#pragma once
#include <chrono>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace EVA::TEST
{
    constexpr char* EVA_TEST_COLOR_RED      = "\x1B[31m";
    constexpr char* EVA_TEST_COLOR_GREEN    = "\x1B[32m";
    constexpr char* EVA_TEST_COLOR_STANDARD = "\033[0m";

    struct FunctionInfo
    {
        void (*function)();
        std::string name;
        std::string category;
        std::string file;
        bool failed;
    };

    class FunctionMap
    {
        inline static std::map<std::string, std::vector<FunctionInfo>> s_Info = std::map<std::string, std::vector<FunctionInfo>>();
        inline static std::vector<FunctionInfo*> s_Failed                     = std::vector<FunctionInfo*>();

      public:
        static size_t Add(void (*function)(), std::string name, std::string category, std::string file)
        {
            auto it = s_Info.find(category);
            if (it == s_Info.end())
            {
                auto inserted = s_Info.insert(std::pair<std::string, std::vector<FunctionInfo>>(category, {}));
                it            = inserted.first;
            }
            it->second.push_back(EVA::TEST::FunctionInfo{ function, name, category, file, false });
            return it->second.size() - 1;
        }

        static void RunAll()
        {
            size_t numTests = 0;
            for (const auto& [category, tests] : s_Info)
            {
                numTests += tests.size();
            }

            std::cout << "Running " << numTests << " tests from " << s_Info.size() << " test suites." << std::endl;

            size_t numPassed = 0;
            size_t qsAll     = 0;

            for (const auto& [category, tests] : s_Info)
            {
                std::cout << tests.size() << " tests in " << category << std::endl;

                size_t numPassedCat = 0;
                size_t qsCat        = 0;

                for (auto& test : tests)
                {
                    std::cout << test.category << "::" << test.name << std::endl;

                    auto startTime = std::chrono::high_resolution_clock::now();
                    test.function();
                    size_t qs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
                    qsCat += qs;

                    if (test.failed)
                    {
                        std::cout << EVA_TEST_COLOR_RED << "  Failed" << EVA_TEST_COLOR_STANDARD << " (" << qs / 1000 << " ms)" << std::endl;
                    }
                    else
                    {
                        numPassedCat++;
                        std::cout << EVA_TEST_COLOR_GREEN << "  Passed" << EVA_TEST_COLOR_STANDARD << " (" << qs / 1000 << " ms)" << std::endl;
                    }
                }

                numPassed += numPassedCat;
                qsAll += qsCat;

                std::cout << (numPassedCat == tests.size() ? EVA_TEST_COLOR_GREEN : EVA_TEST_COLOR_RED) << numPassedCat << " of " << tests.size()
                          << EVA_TEST_COLOR_STANDARD << " tests passed in " << category << " (" << qsCat / 1000 << " ms)" << std::endl
                          << std::endl;
            }

            std::cout << (numPassed == numTests ? EVA_TEST_COLOR_GREEN : EVA_TEST_COLOR_RED) << numPassed << " of " << numTests
                      << EVA_TEST_COLOR_STANDARD << " total tests passed"
                      << " (" << qsAll / 1000 << " ms)" << std::endl;

            for (auto p : s_Failed)
            {
                std::cout << EVA_TEST_COLOR_RED << p->category << " - " << p->name << EVA_TEST_COLOR_STANDARD << std::endl;
            }
        }

        static void Fail(std::string category, size_t index)
        {
            s_Info[category][index].failed = true;
            s_Failed.push_back(&s_Info[category][index]);
        }
    };

    template <typename T, typename = decltype(std::declval<std::ostream&>() << std::declval<const T&>())> 
    std::string ToString(const T& t)
    {
        std::ostringstream out;
        out << t;
        return out.str();
    }

    template <typename T, typename... Ignored> 
    std::string ToString(const T& t, const Ignored&...)
    {
        static_assert(sizeof...(Ignored) == 0, "Incorrect usage: only one parameter allowed");
        return "[Not supported]";
    }

} // namespace EVA::TEST

#define RUN_ALL_TESTS EVA::TEST::FunctionMap::RunAll

#define TEST(CATEGORY, NAME)                                                                                                               \
    struct Test_##CATEGORY##NAME                                                                                                           \
    {                                                                                                                                      \
        static void Run();                                                                                                                 \
        inline static std::string s_Category = #CATEGORY;                                                                                  \
        inline static size_t s_Index         = 0;                                                                                          \
        inline static bool s_Failed          = false;                                                                                      \
        Test_##CATEGORY##NAME() { s_Index = EVA::TEST::FunctionMap::Add(&Run, #NAME, #CATEGORY, __FILE__); }                               \
        inline static void Fail()                                                                                                          \
        {                                                                                                                                  \
            if (!s_Failed)                                                                                                                 \
            {                                                                                                                              \
                s_Failed = true;                                                                                                           \
                EVA::TEST::FunctionMap::Fail(s_Category, s_Index);                                                                         \
            }                                                                                                                              \
        }                                                                                                                                  \
    };                                                                                                                                     \
    inline static Test_##CATEGORY##NAME test_##CATEGORY##NAME;                                                                             \
    void Test_##CATEGORY##NAME::Run()

#define EVA_TEST_PRINT(VALUE) std::cout << "  (" << __LINE__ << ") " << VALUE << std::endl

#define EXPECT_TRUE(C)                                                                                                                     \
    if (!(C))                                                                                                                              \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("The condition is false");                                                                                          \
    }
#define EXPECT_FALSE(C)                                                                                                                    \
    if ((C))                                                                                                                               \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("The condition is true");                                                                                           \
    }

#define EXPECT_EQ(A, B)                                                                                                                    \
    if (!((A) == (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected " << EVA::TEST::ToString((B)) << ", got " << EVA::TEST::ToString((A)));                                   \
    }
#define EXPECT_NE(A, B)                                                                                                                    \
    if (!((A) != (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected the values to not be equal: " << EVA::TEST::ToString((A)));                                               \
    }

#define EXPECT_LT(A, B)                                                                                                                    \
    if (!((A) < (B)))                                                                                                                      \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected " << EVA::TEST::ToString((A)) << " < " << EVA::TEST::ToString((B)));                                      \
    }
#define EXPECT_LE(A, B)                                                                                                                    \
    if (!((A) <= (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected " << EVA::TEST::ToString((A)) << " <= " << EVA::TEST::ToString((B)));                                     \
    }

#define EXPECT_GT(A, B)                                                                                                                    \
    if (!((A) > (B)))                                                                                                                      \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected " << EVA::TEST::ToString((A)) << " == " << EVA::TEST::ToString((B)));                                     \
    }
#define EXPECT_GE(A, B)                                                                                                                    \
    if (!((A) >= (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected " << EVA::TEST::ToString((A)) << " >= " << EVA::TEST::ToString((B)));                                     \
    }

#define EXPECT_STREQ(A, B)                                                                                                                 \
    if (strcmp((A), (B)) != 0)                                                                                                             \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected \"" << EVA::TEST::ToString((B)) << "\", got \"" << EVA::TEST::ToString((A)) << '"');                      \
    }
#define EXPECT_STRNQ(A, B)                                                                                                                 \
    if (strcmp((A), (B)) == 0)                                                                                                             \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected the values to not be equal: \"" << EVA::TEST::ToString((A)) << '"');                                      \
    }

#define EXPECT_MEMEQ(A, B, LENGTH)                                                                                                         \
    if (memcmp((A), (B), (LENGTH)) != 0)                                                                                                   \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected the values to be equal");                                                                                 \
    }
#define EXPECT_MEMNQ(A, B, LENGTH)                                                                                                         \
    if (memcmp((A), (B), (LENGTH)) == 0)                                                                                                   \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Expected the values to not be equal");                                                                             \
    }


#define ASSERT_TRUE(C)                                                                                                                     \
    if (!(C))                                                                                                                              \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. The condition is false");                                                                        \
        return;                                                                                                                            \
    }
#define ASSERT_FALSE(C)                                                                                                                    \
    if ((C))                                                                                                                               \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. The condition is true");                                                                         \
        return;                                                                                                                            \
    }

#define ASSERT_EQ(A, B)                                                                                                                    \
    if (!((A) == (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected " << EVA::TEST::ToString((B)) << ", got " << EVA::TEST::ToString((A)));                 \
        return;                                                                                                                            \
    }
#define ASSERT_NE(A, B)                                                                                                                    \
    if (!((A) != (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected the values to not be equal: " << EVA::TEST::ToString((A)));                             \
        return;                                                                                                                            \
    }

#define ASSERT_LT(A, B)                                                                                                                    \
    if (!((A) < (B)))                                                                                                                      \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected " << EVA::TEST::ToString((A)) << " < " << EVA::TEST::ToString((B)));                    \
        return;                                                                                                                            \
    }
#define ASSERT_LE(A, B)                                                                                                                    \
    if (!((A) <= (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected " << EVA::TEST::ToString((A)) << " <= " << EVA::TEST::ToString((B)));                   \
        return;                                                                                                                            \
    }

#define ASSERT_GT(A, B)                                                                                                                    \
    if (!((A) > (B)))                                                                                                                      \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected " << EVA::TEST::ToString((A)) << " > " << EVA::TEST::ToString((B)));                    \
        return;                                                                                                                            \
    }
#define ASSERT_GE(A, B)                                                                                                                    \
    if (!((A) >= (B)))                                                                                                                     \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected " << EVA::TEST::ToString((A)) << " >= " << EVA::TEST::ToString((B)));                   \
        return;                                                                                                                            \
    }


#define ASSERT_STREQ(A, B)                                                                                                                 \
    if (strcmp((A), (B)) != 0)                                                                                                             \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected \"" << EVA::TEST::ToString((B)) << "\", got \"" << EVA::TEST::ToString((A)) << '"');    \
        return;                                                                                                                            \
    }
#define ASSERT_STRNQ(A, B)                                                                                                                 \
    if (strcmp((A), (B)) == 0)                                                                                                             \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected the values to not be equal: \"" << EVA::TEST::ToString((A)) << '"');                    \
        return;                                                                                                                            \
    }

#define ASSERT_MEMEQ(A, B, LENGTH)                                                                                                         \
    if (memcmp((A), (B), (LENGTH)) != 0)                                                                                                   \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected the values to be equal");                                                               \
        return;                                                                                                                            \
    }
#define ASSERT_MEMNQ(A, B, LENGTH)                                                                                                         \
    if (memcmp((A), (B), (LENGTH)) == 0)                                                                                                   \
    {                                                                                                                                      \
        Fail();                                                                                                                            \
        EVA_TEST_PRINT("Assertion failed. Expected the values to not be equal");                                                           \
        return;                                                                                                                            \
    }

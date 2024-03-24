/**
 * @file gtest-macros.hpp
 * @author lymslive
 * @date 2022-12-28
 * @brief Simulate comparison macro used in gtest.
 * @details 
 * The common `COUT` is similar to `EXPECT_*` serial macro，or `COUTF` is more
 * similar as it only print when statament fails. `COUT_ASSERT` is similar to 
 * `ASSERT_*` serials, which terminate current test case.
 * */
#ifndef GTEST_MACROS_HPP__
#define GTEST_MACROS_HPP__

#include "tinytast.hpp"

#define ASSERT_TRUE(cond)  COUT_ASSERT(cond, true)
#define ASSERT_FALSE(cond) COUT_ASSERT(cond, false)
#define EXPECT_TRUE(cond)  COUTF(cond, true)
#define EXPECT_FALSE(cond) COUTF(cond, false)

#define ASSERT_EQ(a, b) COUT_ASSERT(a, b)
#define EXPECT_EQ(a, b) COUTF(a, b)
#define ASSERT_NE(a, b) COUT_ASSERT(a != b, true)
#define EXPECT_NE(a, b) COUTF(a != b, true)
#define ASSERT_LT(a, b) COUT_ASSERT(a < b, true)
#define EXPECT_LT(a, b) COUTF(a < b, true)
#define ASSERT_LE(a, b) COUT_ASSERT(a <= b, true)
#define EXPECT_LE(a, b) COUTF(a <= b, true)
#define ASSERT_GT(a, b) COUT_ASSERT(a > b, true)
#define EXPECT_GT(a, b) COUTF(a > b, true)
#define ASSERT_GE(a, b) COUT_ASSERT(a >= b, true)
#define EXPECT_GE(a, b) COUTF(a >= b, true)

// compare const char* string
#define ASSERT_STREQ(a, b) COUT_ASSERT(a, std::string(b))
#define EXPECT_STREQ(a, b) COUTF(a, std::string(b))
#define ASSERT_STRNE(a, b) COUT_ASSERT(a != std::string(b), true)
#define EXPECT_STRNE(a, b) COUTF(a != std::string(b), true)

#define ASSERT_STRCASEEQ(a, b) COUT_ASSERT(strcasecmp(a, b), 0)
#define EXPECT_STRCASEEQ(a, b) COUTF(strcasecmp(a, b), 0)
#define ASSERT_STRCASENE(a, b) COUT_ASSERT(strcasecmp(a, b) != 0, true)
#define EXPECT_STRCASENE(a, b) COUTF(strcasecmp(a, b) != 0, true)

#endif /* end of include guard: GTEST_MACROS_HPP__ */

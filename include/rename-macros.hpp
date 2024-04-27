/**
 * @file rename-macros.hpp
 * @author lymslive
 * @date 2024-04-27
 * @brief Rename the most often used macros, provide additional symbols to
 *  access the functionality.
 * */
#pragma once
#ifndef RENAME_MACROS_HPP__
#define RENAME_MACROS_HPP__

#include "tinytast.hpp"
#include "unittast.hpp"
#include "classtast.hpp"
#include "tastargv.hpp"

/// Rename TAST to TEST, for compatibility.
#define DEF_TEST(...) DEF_TAST(__VA_ARGS__)
#define RUN_TEST(...) RUN_TAST(__VA_ARGS__)
#define TIME_TEST(...) TIME_TAST(__VA_ARGS__)

#define G_TESTMGR G_TASTMGR
#define TEST_OPTION TAST_OPTION
#define TEST_ARGV TAST_ARGV

/// Use lower case macro.
#ifndef expect
#define expect(...) COUTF(__VA_ARGS__)
#endif

#ifndef assert_ret
#define assert_ret(...) COUT_ASSERT(__VA_ARGS__)
#endif

/// Define test case by lower case macro.
#ifndef fun_test
#define fun_test(...) DEF_TAST(__VA_ARGS__)
#endif
#ifndef fun_tool
#define fun_tool(...) DEF_TOOL(__VA_ARGS__)
#endif

#ifndef unit_test
#define unit_test(...) DEU_TAST(__VA_ARGS__)
#endif
#ifndef unit_tool
#define unit_tool(...) DEU_TOOL(__VA_ARGS__)
#endif

#ifndef unit_setup
#define unit_setup SETUP
#endif

#ifndef unit_teardown
#define unit_teardown TEARDOWN
#endif

#ifndef class_test
#define class_test(...) DEC_TAST(__VA_ARGS__)
#endif
#ifndef class_tool
#define class_tool(...) DEC_TOOL(__VA_ARGS__)
#endif

#endif /* end of include guard: RENAME_MACROS_HPP__ */

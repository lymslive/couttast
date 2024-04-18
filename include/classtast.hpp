/**
 * @file classtast.hpp
 * @author lymslive
 * @date 2024-04-10
 * @brief Support custome test suite class for DEC_TAST.
 * */
#pragma once
#ifndef CLASSTAST_HPP__
#define CLASSTAST_HPP__

#include "tinytast.hpp"

namespace tast
{

/// The base class which custome test suite class may be derived from.
/// It is encouraged overwrite `setup()` for initial code for each test case,
/// and `teardown()` for any clean up code. The default constructor and
/// destructor can also be used.
struct CTastSuite
{
    void setup() {}
    void teardown() {}
    void run() {}
};

template <typename suiteT>
void tast_suite_run()
{
    suiteT self;
    self.setup();
    self.run();
    self.teardown();
}

} /* tast:: */ 

/// Concate full test case name belong some test suite.
#define STAST_NAME(suite, name) #suite "." #name
/// Concate full test case class identifier belong some test suite.
#define STAST_CLASS(suite, name) CTast_ ## suite ## _ ## name
#define STAST_BUILDER(suite, name) tast_ ## suite ## _ ## name ## _builder
#define STAST_RUNNER(suite, name) tast::tast_suite_run<STAST_CLASS(suite,name)>

/// Define a test case belong to some suite.
#define DEC_TAST_IMPL(suite, name, autoRun, ...) \
    struct STAST_CLASS(suite, name) : public suite { void run(); }; \
    static tast::CTastBuilder STAST_BUILDER(suite,name)(STAST_RUNNER(suite,name), STAST_NAME(suite,name), __FILE__, __LINE__, autoRun, ## __VA_ARGS__); \
    void STAST_CLASS(suite, name)::run()

/// Similar `DEF_TAST` but for custome test suite class.
/// The suite class is inserted as the first argument.
#define DEC_TAST(suite, name, ...) DEC_TAST_IMPL(suite, name, true, ## __VA_ARGS__)

/// Similar `DEF_TOOL` but for custome test suite class.
/// The suite class can override `help()` method for sub-command tool.
#define DEC_TOOL(suite, name, ...) DEC_TAST_IMPL(suite, name, false, ## __VA_ARGS__)

#endif /* end of include guard: CLASSTAST_HPP__ */

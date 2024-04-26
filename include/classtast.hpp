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

#if __cplusplus >= 201103L
#include <type_traits>
#endif

namespace tast
{

/// The base class which custome test suite class may be derived from.
/// It is encouraged overwrite `setup()` for initial code for each test case,
/// and `teardown()` for any clean up code. The default constructor and
/// destructor can also be used.
/// The static `setupFirst()` will be called when the first test case of this
/// suite is executed, and such only called once. The static `teardownLast()`
/// is called after the last test case finished.
/// When compiled with C++11 or above, there is no need to derive from this
/// base class, and any of the four methods can be omitted if no useful
/// action to take.
struct CTastSuite
{
    void setup() {}
    void teardown() {}
    void run() {}
    static void setupFirst() {}
    static void teardownLast() {}
};

/// Helper class for each custome test suite class or file unit or namespace
/// to track how many test cases in it have been defined and runned.
template <typename suiteT>
struct CTastCounter
{
    int tastCount;
    int runCount;
    CTastCounter() : tastCount(0), runCount(0) {}

    static CTastCounter* GetInstance()
    {
        static CTastCounter instance;
        return &instance;
    }

    struct Increase
    {
        Increase(bool autoRun)
        {
            if (autoRun)
            {
                CTastCounter<suiteT>::GetInstance()->tastCount++;
            }
        }
    };
};

template <typename suiteT>
void run_suite_setup_uncheck(suiteT* ptr)
{
    static bool run = false;
    if (run) { return; }
    run = true;
    suiteT::setupFirst();
}

template <typename suiteT>
void run_suite_teardown_uncheck(suiteT* ptr)
{
    if (G_TASTMGR->GetTastRun()->m_autoRun == false)
    {
        return;
    }
    CTastCounter<suiteT>* pInstance = CTastCounter<suiteT>::GetInstance();
    pInstance->runCount++;
    if (pInstance->runCount >= pInstance->tastCount)
    {
        suiteT::teardownLast();
    }
}

#if __cplusplus >= 201103L

template <typename T>
struct HasSuiteSetup
{
    template <typename U>
    static constexpr auto check(U*) -> decltype(&U::setupFirst, std::true_type{});
    template <typename>
    static constexpr std::false_type check(...);
    static constexpr bool value = decltype(check<T>(nullptr))::value;
};

template <typename T>
struct HasSuiteTeardown
{
    template <typename U>
    static constexpr auto check(U*) -> decltype(&U::teardownLast, std::true_type{});
    template <typename>
    static constexpr std::false_type check(...);
    static constexpr bool value = decltype(check<T>(nullptr))::value;
};

template <typename T>
struct HasTestSetup
{
    template <typename U>
    static constexpr auto check(U*) -> decltype(&U::setup, std::true_type{});
    template <typename>
    static constexpr std::false_type check(...);
    static constexpr bool value = decltype(check<T>(nullptr))::value;
};

template <typename T>
struct HasTestTeardown
{
    template <typename U>
    static constexpr auto check(U*) -> decltype(&U::teardown, std::true_type{});
    template <typename>
    static constexpr std::false_type check(...);
    static constexpr bool value = decltype(check<T>(nullptr))::value;
};

template <typename suiteT>
typename std::enable_if<HasSuiteSetup<suiteT>::value, void>::type
/*void*/ run_suite_setup(suiteT* ptr)
{
    run_suite_setup_uncheck<suiteT>(ptr);
}

template <typename suiteT>
typename std::enable_if<!HasSuiteSetup<suiteT>::value, void>::type
/*void*/ run_suite_setup(suiteT* ptr)
{
}

template <typename suiteT>
typename std::enable_if<HasSuiteTeardown<suiteT>::value, void>::type
/*void*/ run_suite_teardown(suiteT* ptr)
{
    run_suite_teardown_uncheck<suiteT>(ptr);
}

template <typename suiteT>
typename std::enable_if<!HasSuiteTeardown<suiteT>::value, void>::type
/*void*/ run_suite_teardown(suiteT* ptr)
{
}

template <typename testT>
typename std::enable_if<HasTestSetup<testT>::value, void>::type
/*void*/ run_test_setup(testT& self)
{
    self.setup();
}

template <typename testT>
typename std::enable_if<!HasTestSetup<testT>::value, void>::type
/*void*/ run_test_setup(testT& self)
{
}

template <typename testT>
typename std::enable_if<HasTestTeardown<testT>::value, void>::type
/*void*/ run_test_teardown(testT& self)
{
    self.teardown();
}

template <typename testT>
typename std::enable_if<!HasTestTeardown<testT>::value, void>::type
/*void*/ run_test_teardown(testT& self)
{
}

#endif

/// The test run function for that derived from a test suite class.
template <typename testT, typename suiteT>
void tast_suite_run()
{
#if __cplusplus >= 201103L
    run_suite_setup<suiteT>(nullptr);
    {
        testT self;
        run_test_setup(self);
        self.run();
        run_test_teardown(self);
    }
    run_suite_teardown<suiteT>(nullptr);
#else
    run_suite_setup_uncheck<suiteT>(NULL);
    {
        testT self;
        self.setup();
        self.run();
        self.teardown();
    }
    run_suite_teardown_uncheck<suiteT>(NULL);
#endif
}

} /* tast:: */ 

/// Concate full test case name belong some test suite.
#define STAST_NAME(suite, name) #suite "." #name
/// Concate full test case class identifier belong some test suite.
#define STAST_CLASS(suite, name) CTast_ ## suite ## _ ## name
#define STAST_BUILDER(suite, name) tast_ ## suite ## _ ## name ## _builder
#define STAST_COUNTER(suite, name) tast_ ## suite ## _ ## name ## _counter
#define STAST_RUNNER(suite, name) tast::tast_suite_run<STAST_CLASS(suite,name), suite>

/// Define a test case belong to some suite.
#define DEC_TAST_IMPL(suite, name, autoRun, ...) \
    struct STAST_CLASS(suite, name) : public suite { void run(); }; \
    static tast::CTastBuilder STAST_BUILDER(suite,name)(STAST_RUNNER(suite,name), STAST_NAME(suite,name), __FILE__, __LINE__, autoRun, ## __VA_ARGS__); \
    static tast::CTastCounter<suite>::Increase STAST_COUNTER(suite,name)(autoRun); \
    void STAST_CLASS(suite, name)::run()

/// Similar `DEF_TAST` but for custome test suite class.
/// The suite class is inserted as the first argument.
#define DEC_TAST(suite, name, ...) DEC_TAST_IMPL(suite, name, true, ## __VA_ARGS__)

/// Similar `DEF_TOOL` but for custome test suite class.
/// The suite class can override `help()` method for sub-command tool.
#define DEC_TOOL(suite, name, ...) DEC_TAST_IMPL(suite, name, false, ## __VA_ARGS__)

#endif /* end of include guard: CLASSTAST_HPP__ */

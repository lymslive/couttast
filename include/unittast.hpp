/**
 * @file unittast.hpp
 * @author lymslive
 * @date 2024-04-25
 * @brief Support group test suite based on file unit or namespace in the file.
 * @details A testing source file or compile unit is considered as a test
 *  module. The test cases defined in one unit may have some relation and share some
 *  expensive resource, such as global(better static) varibles. So an unit
 *  usually need run a `setup()` function once before run any test case, and
 *  it is better to have another opposite `teardown()` function to run after
 *  all tast case.
 * @note The setup and teardown function handle auto tast(like from `DEF_TAST`) 
 *  and manual tool(like from `DEF_TOOL`) differently. So we use `tast` and
 *  `tool` word for each type, and `test` for both.
 * @note Thought it is no encouraged, for large source file, you can have
 *  multiple namespace in one file, and each namespace is can become seperate
 *  test unit. But it is not easy to share test unit in the same namespace
 *  across different source file.
 * */
#pragma once
#ifndef UNITTAST_HPP__
#define UNITTAST_HPP__

#include "tinytast.hpp"

namespace tast
{

/// The state of current unit.
struct CUnitState
{
    int tastCount;      //< Count of tast(but not tool) in this unit defined.
    int runCount;       //< Count of tast finished run.
    voidfun_t setup;    //< Function to run once before the first test/tool.
    voidfun_t teardown; //< Function to run after the last tast.
    bool ready;         //< The setup function has been called.

    CUnitState(voidfun_t fn = NULL)
        : tastCount(0), runCount(0), setup(fn), teardown(NULL), ready(false)
    {}

    /// Add count of tast in the unit.
    struct AddTast
    {
        AddTast(CUnitState& unit, bool autoRun)
        {
            if (autoRun)
            {
                unit.tastCount++;
            }
        }
    };

    /// Define teardown function in the unit.
    struct Defer
    {
        Defer(CUnitState& unit, voidfun_t fn)
        {
            unit.teardown = fn;
        }
    };

    /// Check to run setup function once before any tast/tool.
    void CheckSetupFirst()
    {
        if (!ready && setup != NULL)
        {
            setup();
            ready = true;
        }
    }

    /// Check to run teardown function after all tast.
    void CheckTeardownLast()
    {
        if (teardown == NULL)
        {
            return;
        }
        if (G_TASTMGR->GetTastRun()->m_autoRun == false)
        {
            return;
        }
        runCount++;
        if (runCount >= tastCount)
        {
            teardown();
            ready = false;
        }
    }
};

/// The base class wrapper for test in current refered unit.
struct CUnitTast
{
    CUnitState& unit;

    CUnitTast(CUnitState& refer) : unit(refer)
    {}
};

/// The running entrance function for each test.
template <typename testT>
void run_unit_tast()
{
    testT self;
    self.unit.CheckSetupFirst();
    self.run();
    self.unit.CheckTeardownLast();
}

} /* tast:: */ 

/// Setup current unit for following test cases.
/// Must use this macro the eable test unit tracking.
#define SETUP() \
    static void s_setup();                    \
    static tast::CUnitState s_unit(s_setup); \
    void s_setup()

/// Define optional teardown function for current test unit.
#define TEARDOWN() \
    static void s_teardown(); \
    static tast::CUnitState::Defer s_unitDefer(s_unit, s_teardown); \
    void s_teardown()


/// Helper macros to simply the unit test case definition.
#define UTAST_COUNTER(name) tast_##name##_counter
#define UTAST_CLASS(name) CUnitTast_ ## name
#define UTAST_RUNNER(name) tast::run_unit_tast<UTAST_CLASS(name)>

/// Define a test case in current unit.
#define DEU_TAST_IMPL(name, autoRun, ...) \
    static void TAST_RUNNER(name)(); \
    struct UTAST_CLASS(name) : public tast::CUnitTast \
    { \
        UTAST_CLASS(name)() : tast::CUnitTast(s_unit) {} \
        void run(); \
    }; \
    static tast::CTastBuilder TAST_BUILDER(name)(UTAST_RUNNER(name), #name, __FILE__, __LINE__, autoRun, ## __VA_ARGS__); \
    static tast::CUnitState::AddTast UTAST_COUNTER(name)(s_unit, autoRun); \
    void UTAST_CLASS(name)::run()

/// Define an auto tast case. Must use after SETUP unit.
#define DEU_TAST(name, ...) DEU_TAST_IMPL(name, true, ## __VA_ARGS__)

/// Define a manual tool case. Must use after SETUP unit.
#define DEU_TOOL(name, ...) DEU_TAST_IMPL(name, false, ## __VA_ARGS__)

#endif /* end of include guard: UNITTAST_HPP__ */

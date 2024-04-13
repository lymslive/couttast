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

/// The base class which custome test suite class should be derived from.
/// It is encouraged override `setup()` for initial code for each test case,
/// and `teardown()` for any clean up code. The default constructor and
/// destructor can also be used, but is not encouraged.
struct CTastSuite : public CTastCase
{
    virtual void setup() {}
    virtual void teardown() {}
    virtual void body() = 0;
    virtual void run()
    {
        this->setup();
        this->body();
        this->teardown();
    }
};

} /* tast:: */ 

/// Concate full test case name belong some test suite.
#define STEST_NAME(suite, name) #suite "." #name
/// Concate full test case class identifier belong some test suite.
#define STEST_CLASS(suite, name) CTast_ ## suite ## _ ## name

/// Define a test case belong to some suite.
#define DEC_TAST_IMPL(suite, name, autoRun, ...) \
    class STEST_CLASS(suite, name) : public suite \
    { \
    public: \
        virtual void body(); \
    private: \
        static tast::CTastBuilder<STEST_CLASS(suite, name)> m_builder; \
    }; \
    tast::CTastBuilder<STEST_CLASS(suite, name)> STEST_CLASS(suite, name)::m_builder(STEST_NAME(suite, name), __FILE__, __LINE__, autoRun, ## __VA_ARGS__); \
    void STEST_CLASS(suite, name)::body()

/// Similar `DEF_TAST` but for custome test suite class.
/// The suite class is inserted as the first argument.
#define DEC_TAST(suite, name, ...) DEC_TAST_IMPL(suite, name, true, ## __VA_ARGS__)

/// Similar `DEF_TOOL` but for custome test suite class.
/// The suite class can override `help()` method for sub-command tool.
#define DEC_TOOL(suite, name, ...) DEC_TAST_IMPL(suite, name, false, ## __VA_ARGS__)

#endif /* end of include guard: CLASSTAST_HPP__ */

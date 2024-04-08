#pragma once
#ifndef CLASSTAST_HPP__
#define CLASSTAST_HPP__

#include "tinytast.hpp"

namespace tast
{
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

#define DEC_TAST(suite, name, ...) DEC_TAST_IMPL(suite, name, true, ## __VA_ARGS__)
#define DEC_TOOL(suite, name, ...) DEC_TAST_IMPL(suite, name, false, ## __VA_ARGS__)

#endif /* end of include guard: CLASSTAST_HPP__ */

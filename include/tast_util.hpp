#ifndef TAST_UTIL_HPP__
#define TAST_UTIL_HPP__
// DEPRECATED: too simple version v0.1, you may use higher one for larger test.
// Please use tinytast.hpp for header only, 
// or couttast.h when link with libcouttast.a
#include <cstdio>
#include <iostream>

namespace tast
{

struct STastState
{
    const char* currentFun = nullptr;
    int currentFail = 0;

    int passedFun = 0;
    int failedFun = 0;

    void begin_fun(const char* funName)
    {
        currentFun = funName;
        currentFail = 0;
    }

    static STastState* instance()
    {
        static STastState inst;
        return &inst;
    }

private:
    STastState() {}
};

inline
const char* pass_str(bool tf, bool eachCout = false)
{
    if (eachCout)
    {
        return tf ? "OK" : "NO";
    }
    return tf ? "PASS" : "FAIL";
}

inline
void cout_value(bool tf)
{
    printf("%s", tf ? "ture" : "false");
}

template <typename T>
void cout_value(const T& value)
{
    std::cout << value;
}

template <typename T>
void cout(const char* strExpr, const T& valExpr)
{
    printf("|| %s =~? ", strExpr);
    cout_value(valExpr);
    printf("\n");
}


template <typename U, typename V>
void cout(const char* strExpr, const U& valExpr, const V& valExpect)
{
    printf("|| %s =~? ", strExpr);
    cout_value(valExpr);

    bool bPass = (valExpr == valExpect);
    const char* strPass = pass_str(bPass, true);
    if (!bPass)
    {
        STastState::instance()->currentFail++;
    }

    printf(" [%s]\n", strPass);
    if (!bPass)
    {
        printf("!! Expect: ");
        cout_value(valExpect);
        printf("\n");
    }
}

typedef void (*PFTAST)();

inline
void do_tast_fun(const char* funName, PFTAST fun)
{
    STastState* pTast = STastState::instance();
    pTast->begin_fun(funName);

    printf("## %s\n", funName);
    fun();

    if (pTast->currentFail == 0)
    {
        printf("<< [%s] %s\n", pass_str(true), funName);
        pTast->passedFun++;
    }
    else
    {
        printf("<< [%s] %d IN %s\n", pass_str(false), pTast->currentFail, funName);
        pTast->failedFun++;
    }

    printf("\n");
}

inline
int report_tast_summary()
{
    STastState* pTast = STastState::instance();
    printf("## Summary\n");
    printf("<< [%s] %d\n", pass_str(true), pTast->passedFun);
    printf("<< [%s] %d\n", pass_str(false), pTast->failedFun);
    return pTast->failedFun;
}

};

// simple define
#if 0
#define COUT(expr) std::cout << "|| " << #expr << " =~? " << (expr) << std::endl;
#define TAST(fun) \
    std::cout << "## " << #fun << std::endl; \
    fun(); \
    std::cout << std::endl;
#endif

#define COUT(expr, ...) tast::cout(#expr, (expr), ## __VA_ARGS__)
#define CODE(statement) printf("// %s\n", #statement); statement
#define DESC(msg, ...) printf("-- "); printf(msg, ## __VA_ARGS__); printf("\n")
#define TAST(fun) tast::do_tast_fun(#fun, fun)
#define TAST_RESULT tast::report_tast_summary()

#endif /* end of include guard: TAST_UTIL_HPP__ */

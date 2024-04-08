#include "tinytast.hpp"
#include "classtast.hpp"

struct SComplex
{
    int real = 0;
    int image = 0;
};

DEF_TOOL(concat_dot, "macro to contcat with dot")
{
// #define DOT .
// #define CONCAT_WITH_DOT(a, b) a ## . ## b
// Note: not possible
//     SComplex number;
//     int real = CONCAT_WITH_DOT(number, real);
//     COUT(real);
//     COUT(number.real);
// #undef CONCAT_WITH_DOT

#define CONCAT_WITH_DOT(a, b) a##b
    int part1 = 42;
    int part2 = 99;
    int part1part2 = 4299;
    int combined = CONCAT_WITH_DOT(part1, part2);
    DESC("Combined value: %d", combined);
#undef CONCAT_WITH_DOT

#define CONCAT_WITH_DOT(a, b) #a "." #b
    const char* pszCombined = CONCAT_WITH_DOT(part1, part2);
    DESC("Combined string: %s", pszCombined);
#undef CONCAT_WITH_DOT
}

struct MySuite : public tast::CTastSuite
{
    int intValue = 0;
    std::string strValue;
    SComplex complexValue;

    MySuite()
    {
        // DESC("MySuite constructor");
        // would run before main
        intValue = 100;
    }
    ~MySuite()
    {
        // DESC("MySuite destructor");
        // would run after main
    }

    virtual void setup()
    {
        DESC("MySuite::setup");
        strValue = "abcdefg";
    }

    virtual void teardown()
    {
        DESC("MySuite::teardown");
    }
};

DEC_TAST(MySuite, aaa, "test suite by DEC_TAST")
{
    COUT(intValue, 100);
    COUT(strValue, "abcdefg");
    strValue[0] = 'A';
    COUT(strValue, "Abcdefg");
}

DEC_TOOL(MySuite, bbb, "test suite by DEC_TOOL")
{
    COUT(intValue, 100);
    COUT(strValue, "abcdefg");
    strValue[1] = 'B';
    COUT(strValue, "aBcdefg");
}

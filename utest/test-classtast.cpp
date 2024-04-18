#include "tinytast.hpp"
#include "classtast.hpp"

struct SComplex
{
    int real;
    int image;
};

struct MySuite : public tast::CTastSuite
{
    int intValue;
    std::string strValue;
    SComplex complexValue;

    MySuite()
    {
        DESC("MySuite constructor");
        intValue = 100;
    }
    ~MySuite()
    {
        DESC("MySuite destructor");
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

struct add : public tast::CTastSuite
{
    int left;
    int right;

    add() : left(0), right(0)
    {
    }

    void setup()
    {
        if (TAST_OPTION.count("help"))
        {
            help();
            exit(0);
        }

        std::string strLeft = TAST_OPTION["left"];
        std::string strRight = TAST_OPTION["right"];
        left = atoi(strLeft.c_str());
        right = atoi(strRight.c_str());
    }

    void help() const
    {
        std::string output;
        output += "custome help for usage:\n";
        output += "add.exe --left=? --right=?";
        std::cout << output << std::endl;
    }
};

DEC_TOOL(add, exe, "sample tool for add two number: --left=, --right=")
{
    int result = left + right;
    std::cout << "result: " << result << std::endl;
}

DEF_TAST(duplicate, "may duplicate test name in another file")
{
    COUT(4+4, 8);
}

DEC_TAST(MySuite, duplicate, "duplicate test name in suite")
{
    COUT(5+5, 10);
}

DEC_TAST(add, duplicate, "duplicate test name in another suite")
{
    COUT(6+6, 12);
}


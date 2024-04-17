#include "tinytast.hpp"
#include "classtast.hpp"

struct SComplex
{
    int real = 0;
    int image = 0;
};

struct MySuite : public tast::CTastSuite
{
    int intValue = 0;
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
    int left = 0;
    int right = 0;

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

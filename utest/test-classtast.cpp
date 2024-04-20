#include "tinytast.hpp"
#include "classtast.hpp"
#include "tastargv.hpp"

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
        if (!TAST_ARGV["help"].empty())
        {
            help();
            exit(0);
        }

        BIND_ARGV(left);
        BIND_ARGV(right);
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

    int sum = 0;
    int num = 0;
    for (int i = 1; BIND_ARGV(num, i); ++i)
    {
        sum += num;
    }
    COUT(sum);
}

DEC_TOOL(add, fexe, "sample tool for add two float number: --left=, --right=")
{
    double left;
    double right;
    BIND_ARGV(left);
    BIND_ARGV(right);
    double result = left + right;
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

DEF_TAST(tastargv, "test argv managemant")
{
    tast::CTastMgr stTastMgr;
    tast::CTastArgv argv(&stTastMgr);

    tast::CliArgument& argument = argv.Argument();
    tast::CliOption& option = argv.Option();

    argument.push_back("hello");
    argument.push_back("world");

    option["xyz"] = "123";
    option["section.xyz"] = "45.6";

    COUT(argv[0], "hello");
    COUT(argv[1], "world");
    COUT(argv["xyz"], "123");
    COUT(argv["section.xyz"], "45.6");
    COUT(argv.HasKey("xyz"), true);
    COUT(argv.HasKey(".xyz"), false);

    std::string val;
    COUT(argv.BindValue(val, "xzy"), false);
    COUT(argv.BindValue(val, "xyz"), true);
    COUT(val, "123");

    int num;
    COUT(argv.BindValue(num, "xyz"), true);
    COUT(num, 123);

    double dnum;
    COUT(argv.BindValue(dnum, "section.xyz"), true);
    COUT(dnum, 45.6);
}

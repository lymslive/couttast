#include "tinytast.hpp"
#include "logprint.hpp"

struct SDate
{
    int year = 2000;
    int month = 1;
    int day = 1;

    std::string to_string() const
    {
        std::string buffer;
        buffer += std::to_string(year);
        buffer += "-";
        buffer += std::to_string(month);
        buffer += "-";
        buffer += std::to_string(day);
        return buffer;
    }
};

DEF_TAST(tostring, "tast utd::to_string")
{
    COUT(utd::to_string(100), "100");
    DESC("cannot specify format double well");
    COUT(std::to_string(3.14));
    COUT(utd::to_string(3.14), "3.140000");
    COUT(utd::to_string(3.14e8));

    std::string str("cpp string");
    COUT(utd::to_string(str), str);

    COUT(utd::to_string(SDate()), "2000-1-1");
    SDate date;
    COUT(utd::to_string(&date));

    std::string buffer;
    utd::to_string(buffer, 3.14);
    COUT(buffer, "3.140000");
    utd::to_string(buffer, 159);
    COUT(buffer, "3.140000159");
    utd::to_string(buffer, "=pi");
    COUT(buffer, "3.140000159=pi");
}

DEF_TAST(strf, "tast utd::strf")
{
    std::string buffer;
    buffer = utd::strf("%.2f, %.2f", 3.14, 6.288);
    COUT(buffer, "3.14, 6.29");

    buffer = utd::strF("%.2f, %.2f", 3.14, 6.288);
    COUT(buffer, "3.14, 6.29");
}

DEF_TAST(strhex, "tast utd::strhex")
{
    std::string buffer;
    SDate date;
    char pszdata[] = "2000-1-1";
    COUT(utd::to_string(&date));
    COUT(utd::to_string(date), pszdata);
    buffer = utd::strhex(date);
    COUT(buffer);
    buffer = utd::strhex(pszdata);
    COUT(buffer, "32 30 30 30 2d 31 2d 31 00");
    buffer = utd::strheX(pszdata);
    COUT(buffer, "32 30 30 30 2D 31 2D 31 00");

    COUT(utd::strhex(0));
    COUT(utd::strhex(0x01ABCDEF), "01 ab cd ef");
    COUT(utd::strheX(0xABCDEF), "00 AB CD EF");
    COUT(utd::strheX(0xABCD), "00 00 AB CD");
    COUT(utd::strheX((short)0xABCD), "AB CD");
    COUT(utd::little_endian());
}

DEF_TAST(printf, "tast utd::printf to stdout")
{
    DESC("printf with not any args");
    utd::printf("a simple string\n");

    DESC("printf with only one arg");
    utd::printf("int: %d\n", 100);

    DESC("printf with more args");
    utd::printf("int: %d; double: %f\n", 100, 3.14, "cpp");
    utd::printf("int: %d; double: %f; str: %s\n", 100, 3.14, "cpp");

    DESC("printf with %% at end");
    utd::printf("int: %", 100);
    utd::printf("int: %", 200);
    utd::printf("%|%", true, false);
    utd::printf("\n", 300);

    DESC("printf with any postfix after %%");
    utd::printf("int: %g; double: %g; str: %g\n", 100, 3.14, "cpp");
    utd::printf("int: %1; double: %2; str: %3\n", 100, 3.14, "cpp");
    utd::printf("int: %; double: %; str: %\n", 100, 3.14, "cpp");

    DESC("use log macro");
    COUT_LOG("int: %; double: %; str: %\n", 100, 3.14, "cpp");
}

DEF_TAST(sprintf, "tast utd::sprintf")
{
    std::string buffer;

    utd::sprintf(buffer, "a simple string");
    COUT(buffer, "a simple string");

    buffer.clear();
    utd::sprintf(buffer, "int:%d", 100);
    COUT(buffer, "int:100");

    buffer.clear();
    utd::sprintf(buffer, "int:%d; double:%f; str:%s", 100, 3.14, "cpp");
    COUT(buffer, "int:100; double:3.140000; str:cpp");

    buffer.clear();
    utd::sprintf(buffer, "int:%; ", 100);
    utd::sprintf(buffer, "int:%; ", 200);
    utd::sprintf(buffer, "%|%", true, false);
    COUT(buffer, "int:100; int:200; true|false");

    buffer.clear();
    utd::sprintf(buffer, "int:%; double:%; str:%", 100, 3.14, "cpp");
    COUT(buffer, "int:100; double:3.140000; str:cpp");

    buffer.clear();
    utd::sprintf(buffer, "Date:%v", SDate());
    COUT(buffer, "Date:2000-1-1");

    DESC("use old sprintf style in argument which need specical format");
    buffer.clear();
    utd::sprintf(buffer, "int:%; double:%; str:[%]",
            utd::strf("0x%.2x", 100), utd::strf("%.2f", 3.14), utd::strhex("cpp"));
    COUT(buffer, "int:0x64; double:3.14; str:[63 70 70 00]");
}

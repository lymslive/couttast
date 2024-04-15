#include "couttast.h"
// test src/file-compare.cpp and src/xmltast.cpp
#include "file-compare.h"
#include "xmltast.h"
#include "extra-macros.hpp"

#include <algorithm>
#include <ctype.h>

using tast::xml::CTestData;
using tast::xml::CTestReader;
using tast::xml::CTestWriter;

DEF_TAST(xml_same, "read xml, write back the same and compare")
{
    const char* inputFile = "sample.xml";
    const char* outputFile = "sample.tmp";

    CTestReader reader(inputFile);
    CTestWriter writer(outputFile);

    int count = 0;
    CTestData test;
    while (reader.NextTest(test))
    {
        count++;
        DESC("testcase: %d", count);
        if (count == 1)
        {
            COUT(test.id, "C1");
            COUT(test.description, "d1 d1");
            COUT(test.input, "      aaaaaaaaaa.");
            COUT(test.data, "      ZZZZZZZZZZ.");
            COUT(test.expect, "      AAAAAAAAAA.");
        }
        else if (count == 2)
        {
            COUT(test.id, "C2");
            COUT(test.description, "d2 d2");
            COUT(test.input, "      bbbbbbbbbb.");
            COUT(test.data, "      ZZZZZZZZZZ.");
            COUT(test.expect, "      BBBBBBBBBB.");
        }

        writer.WriteTest(test);
    }

    COUT(count, 4);

    writer.Close();
    bool same = tast::macro::cout_file(SRC_LOCATION, inputFile, outputFile);
    COUT(same, true);
    COUT_FILE(inputFile, outputFile);
}

DEF_TAST(xml_diff, "read xml, transform, write back and compare diff")
{
    const char* inputFile = "sample.xml";
    const char* outputFile = "sample.tmp";

    CTestReader reader(inputFile);
    CTestWriter writer(outputFile);

    int count = 0;
    CTestData test;
    while (reader.NextTest(test))
    {
        count++;
        DESC("testcase: %d", count);
        std::string& input = test.input;
        std::string output = input;
        std::transform(input.begin(), input.end(), output.begin(), ::toupper);
        COUT(output);

        writer.WriteTest(test, &output);
    }

    COUT(count, 4);

    writer.Close();
    bool same = tast::macro::cout_file(SRC_LOCATION, inputFile, outputFile);
    COUT(same, false);
    COUT_FILE(inputFile, outputFile);

    COUT_FILE(outputFile);
    COUT_ERROR(2); // clear 2 failed
}

bool FnTestCallback(CTestData& test)
{
    if (test.command != nullptr)
    {
        return true;
    }
    std::string& input = test.input;
    std::string output = input;
    std::transform(input.begin(), input.end(), output.begin(), ::toupper);
    return COUT(output, test.expect);
}

DEF_TAST(xml_testor, "xml test reader and driver")
{
    const char* inputFile = "sample.xml";
    CTestReader reader(inputFile);

    int failed = reader.TestRun(FnTestCallback);
    COUT(failed, 0);
}

DEF_TAST(xml_command, "xml test reader and command driver")
{
    const char* inputFile = "sample.xml";
    CTestReader reader(inputFile);

    int failed = reader.TestCommand();
    COUT(failed, 0);
}

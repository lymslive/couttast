#include "couttast.h"
// test src/file-compare.cpp and src/test-reader.cpp
#include "file-compare.h"
#include "test-reader.h"
#include "extra-macros.hpp"

#include <algorithm>
#include <ctype.h>

using tast::util::CTestData;
using tast::util::CTestReader;
using tast::util::CTestWriter;

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

    COUT(count, 2);

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

    COUT(count, 2);

    writer.Close();
    bool same = tast::macro::cout_file(SRC_LOCATION, inputFile, outputFile);
    COUT(same, false);
    COUT_FILE(inputFile, outputFile);

    COUT_FILE(outputFile);
    COUT_ERROR(2); // clear 2 failed
}

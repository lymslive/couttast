#include "file-compare.h"

#include <fstream>

namespace tast
{
namespace macro
{

bool cout_file(const tast::CLocation& location, const std::string& file)
{
    std::ifstream afile(file);
    if (!afile.is_open())
    {
        tast::CStatement(location, file.c_str()).cout("open error");
        return false;
    }

    int lineNumber = 0;
    std::string line;
    while(std::getline(afile, line))
    {
        G_TASTMGR->Print(tast::COUT_BIT_COUT, line.c_str());
        ++lineNumber;
    }

    std::string summary = std::to_string(lineNumber);
    summary += " lines";
    tast::CStatement(location, file.c_str()).cout(summary);
    return true;
}

bool cout_file(const tast::CLocation& location, const std::string& file, const std::string& expect)
{
    std::ifstream afile(file);
    if (!afile.is_open())
    {
        tast::CStatement(location, file.c_str()).cout("open error", "open read");
        return false;
    }

    std::ifstream bfile(expect);
    if (!bfile.is_open())
    {
        tast::CStatement(location, expect.c_str()).cout("open error", "open read");
        return false;
    }

    std::string result = expect;
    result += ":";

    std::string aline, bline;
    int lineNumber = 0;

    std::getline(afile, aline);
    std::getline(bfile, bline);
    while (afile && bfile)
    {
        ++lineNumber;
        if (aline != bline) {
            G_TASTMGR->Print(tast::COUT_BIT_FAIL, "Files differ at line: %d, - %s, + %s", lineNumber, file.c_str(), expect.c_str());
            G_TASTMGR->Print(tast::COUT_BIT_FAIL, "- %s", aline.c_str());
            G_TASTMGR->Print(tast::COUT_BIT_FAIL, "+ %s", bline.c_str());

            result += std::to_string(lineNumber);
            tast::CStatement(location, file.c_str()).cout(result, expect, false);
            return false;
        }
        std::getline(afile, aline);
        std::getline(bfile, bline);
    }

    if (afile.eof() != bfile.eof())
    {
        result += std::to_string(lineNumber + 1);
        tast::CStatement(location, file.c_str()).cout(result, expect, false);
        return false;
    }

    result += std::to_string(lineNumber);
    tast::CStatement(location, file.c_str()).cout(result, expect, true);
    return true;
}

}
} // end of tast::macro::

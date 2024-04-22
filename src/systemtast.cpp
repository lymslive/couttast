#include "systemtast.h"

#include <array>
#include <regex>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace tast
{

namespace util
{

void trim_string(const std::string& src, const char*& first, const char*& last)
{
    size_t size = src.size();
    size_t begin = 0;
    while (begin < src.size() && ::isspace(src[begin]))
    {
        begin++;
    }
    first = &src[begin];

    size_t end = size - 1;
    while (end > begin && ::isspace(src[end]))
    {
        end--;
    }

    last = &src[end + 1];
}

bool match_string(const std::string& output, const std::string& expect, int flag)
{
    if (output.empty())
    {
        return expect.empty();
    }
    else if (expect.empty())
    {
        return output.empty();
    }

    if (flag == STRING_MATCH_TRIM)
    {
        const char* outputFirst = nullptr;
        const char* outputLast = nullptr;
        const char* expectFirst = nullptr;
        const char* expectLast = nullptr;

        trim_string(output, outputFirst, outputLast);
        trim_string(expect, expectFirst, expectLast);

        size_t outputLength = outputLast - outputFirst;
        size_t expectLength = expectLast - expectFirst;

        if (outputLength != expectLength)
        {
            return false;
        }
        else if (outputLength == 0)
        {
            return true;
        }

        return ::memcmp(outputFirst, expectFirst, outputLength) == 0;
    }
    else if (flag == STRING_MATCH_EQUAL)
    {
        return output == expect;
    }
    else if (flag == STRING_MATCH_SUBSTR)
    {
        return output.find(expect) != std::string::npos;
    }
    else if (flag == STRING_MATCH_WILD)
    {
        std::string regexp;
        for (char ch : expect)
        {
            if (ch == '?')
            {
                regexp += ".";
            }
            else if (ch == '*')
            {
                regexp += ".*";
            }
            else
            {
                regexp += ch;
            }
        }
        if (regexp.size() == expect.size())
        {
            return output.find(expect) != std::string::npos;
        }
        else
        {
            return std::regex_search(output, std::regex(regexp));
        }
    }
    else if (flag == STRING_MATCH_REGEXP)
    {
        return std::regex_search(output, std::regex(expect));
    }

    return false;
}

} // end of util::

namespace system
{

int Execute(const std::string& command, std::string& output)
{
    std::array<char, 128> buffer;
    FILE* pipe = ::popen(command.c_str(), "r");

    if (pipe == nullptr)
    {
        return -1;
    }

    while (::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        output += buffer.data();
    }

    int exitCode = pclose(pipe);
    exitCode = WEXITSTATUS(exitCode);
    return exitCode;
}

} // end of system::

namespace macro
{

bool cout_system(const tast::CLocation& location, const std::string& command)
{
    std::string output;
    int exitCode = tast::system::Execute(command, output);
    tast::CStatement(location, command.c_str()).cout(exitCode, 0);
    if (!output.empty() && G_TASTMGR->CoutMask(COUT_BIT_COUT))
    {
        G_TASTMGR->Print(output.c_str());
    }
	return exitCode == 0;
}

bool cout_system(const tast::CLocation& location, const std::string& command, int exitCodeExpect)
{
    std::string output;
    int exitCode = tast::system::Execute(command, output);
    tast::CStatement(location, command.c_str()).cout(exitCode, exitCodeExpect);
    if (!output.empty() && G_TASTMGR->CoutMask(COUT_BIT_COUT))
    {
        G_TASTMGR->Print(output.c_str());
    }
	return exitCode == exitCodeExpect;
}

bool cout_system(const tast::CLocation& location, const std::string& command, const std::string& outputExpect, int flagMatch/* = 0*/)
{
    std::string output;
    int exitCode = tast::system::Execute(command, output);
    tast::CStatement(location, command.c_str()).cout(exitCode, 0);
    if (!output.empty() && G_TASTMGR->CoutMask(COUT_BIT_COUT))
    {
        G_TASTMGR->Print(output.c_str());
    }

    bool outputMatch = tast::util::match_string(output, outputExpect, flagMatch);
    if (!outputMatch)
    {
        tast::CStatement(location, "command output").cout("^^^", outputExpect, false);
    }

	return exitCode == 0 && outputMatch;
}

bool cout_system(const tast::CLocation& location, const std::string& command, int exitCodeExpect, const std::string& outputExpect, int flagMatch/* = 0*/)
{
    std::string output;
    int exitCode = tast::system::Execute(command, output);
    tast::CStatement(location, command.c_str()).cout(exitCode, exitCodeExpect);
    if (!output.empty() && G_TASTMGR->CoutMask(COUT_BIT_COUT))
    {
        G_TASTMGR->Print(output.c_str());
    }

    bool outputMatch = tast::util::match_string(output, outputExpect, flagMatch);
    if (!outputMatch)
    {
        tast::CStatement(location, "command output").cout("^^^", outputExpect, false);
    }

    return exitCode == exitCodeExpect && outputMatch;
}

} // endof macro::
} /* tast::macro */ 

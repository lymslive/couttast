#include "xmltast.h"
#include "tinytast.hpp"
#include "coutstd.hpp"
#include "systemtast.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <unistd.h>

namespace tast
{
namespace xml
{

CTestData::~CTestData()
{
    clear();
}

void CTestData::clear()
{
    id.clear();
    description.clear();
    input.clear();
    data.clear();
    expect.clear();
    if (command != nullptr)
    {
        delete command;
        command = nullptr;
    }
}

/* ---------------------------------------------------------------------- */

class CTestReader::Impl
{
public:
    Impl(const char* inputFile) : m_fileName(inputFile), m_stream(inputFile)
    {}

    bool NextTest(CTestData& test);

private:
    /// Simple implement using find ans substr methods of std::string.
    /// Not allow extra space around `=` for attribute nor `<>` for tag.
    bool ExtractTest(CTestData& test, std::stringstream& buffer);
    std::string ExtractAttribute(const std::string& line, const std::string& attr);
    std::string ExtractContent(std::stringstream& stream, const std::string& tag);

private:
    std::string m_fileName;
    std::ifstream m_stream;
};

bool CTestReader::Impl::NextTest(CTestData& test)
{
    test.clear();
    std::string line;
    std::stringstream buffer;

    // Read lines until </testcase> tag is found
    while (std::getline(m_stream, line))
    {
        if (line.find("</testcase>") != std::string::npos)
        {
            // Extract case information from buffer
            return ExtractTest(test, buffer);
        }
        else
        {
            buffer << line << '\n';
        }
    }

    return false; // End of file or error
}

bool CTestReader::Impl::ExtractTest(CTestData& test, std::stringstream& stream)
{
    bool found = false;
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.find("<testcase") != std::string::npos)
        {
            test.id = ExtractAttribute(line, "id");
            test.description = ExtractAttribute(line, "description");
            found = true;
        }
        else if (found)
        {
            if (line.find("<input>") != std::string::npos)
            {
                test.input = ExtractContent(stream, "input");
            }
            else if (line.find("<data>") != std::string::npos)
            {
                test.data = ExtractContent(stream, "data");
            }
            else if (line.find("<expect>") != std::string::npos)
            {
                test.expect = ExtractContent(stream, "expect");
            }
            else if (line.find("<command") != std::string::npos)
            {
                std::string cmdLine = ExtractContent(stream, "command");
                if (!cmdLine.empty())
                {
                    test.command = new CTestCommand;
                    test.command->cmdLine = std::move(cmdLine);
                    std::string exitCode = ExtractAttribute(line, "exitCode");
                    if (exitCode.empty())
                    {
                        test.command->exitCode = 0;
                    }
                    else
                    {
                        test.command->exitCode = ::atoi(exitCode.c_str());
                    }
                    test.command->workDir = ExtractAttribute(line, "workDir");
                    test.command->match = ExtractAttribute(line, "match");
                }
            }
        }
    }

    return found;
}

std::string CTestReader::Impl::ExtractAttribute(const std::string& line, const std::string& attr)
{
    std::string result;
    size_t pos = line.find(attr + "=\"");
    if (pos != std::string::npos)
    {
        pos += attr.size() + 2; // Move to start of attribute value
        size_t endPos = line.find('"', pos); // Find end of attribute value
        if (endPos != std::string::npos)
        {
            result = line.substr(pos, endPos - pos);
        }
    }
    return result;
}

std::string CTestReader::Impl::ExtractContent(std::stringstream& stream, const std::string& tag)
{
    std::string content;
    std::string line;

    std::string closingTag = "</" + tag + ">";

    // Read lines until corresponding closing tag is found
    while (std::getline(stream, line))
    {
        if (line.find(closingTag) != std::string::npos)
        {
            break;
        }
        content += line + '\n';
    }

    // Remove trailing newline if any
    if (!content.empty() && content.back() == '\n')
    {
        content.pop_back();
    }

    return content;
}

/* ---------------------------------------------------------------------- */

CTestReader::CTestReader(const char* inputFile) : m_pImpl(new Impl(inputFile))
{
}

CTestReader::~CTestReader()
{
    if (m_pImpl != nullptr)
    {
        delete m_pImpl;
        m_pImpl = nullptr;
    }
}

bool CTestReader::NextTest(CTestData& test)
{
    test.clear();
    return m_pImpl->NextTest(test);
}

int CTestReader::TestRun(FnTest fn)
{
    std::string xmlFilter = TAST_OPTION["xml-filter"];

    std::vector<std::string> failed;
    int index = 0;

    CTestData test;
    while (NextTest(test))
    {
        if (!xmlFilter.empty() && test.id.find(xmlFilter) == std::string::npos)
        {
            continue;
        }

        DESC("case %d: %s %s", ++index, test.id.c_str(), test.description.c_str());

        if (false == fn(test))
        {
            failed.push_back(test.id);
        }
    }

    COUT(failed.size());
    COUT(failed);
    return failed.size();
}

bool test_command(CTestData& test)
{
    if (test.command == nullptr)
    {
        DESC("skip as no command test data");
        return true;
    }

    char* oldPwd = nullptr;
    if (test.command->workDir.empty() == false)
    {
        oldPwd = ::get_current_dir_name();
        ::chdir(test.command->workDir.c_str());
    }

    int flag = tast::STRING_MATCH_TRIM;
    if (test.command->match.empty() == false)
    {
        if (test.command->match == "equal")
        {
            flag = tast::STRING_MATCH_EQUAL;
        }
        else if (test.command->match == "substr")
        {
            flag = tast::STRING_MATCH_SUBSTR;
        }
        else if (test.command->match == "wild")
        {
            flag = tast::STRING_MATCH_WILD;
        }
        else if (test.command->match == "regexp")
        {
            flag = tast::STRING_MATCH_REGEXP;
        }
    }

    bool pass = false;
    if (test.expect.empty())
    {
        pass = COUT_SYSTEM(test.command->cmdLine, test.command->exitCode);
    }
    else
    {
        pass = COUT_SYSTEM(test.command->cmdLine, test.command->exitCode, test.expect, flag);
    }

    if (oldPwd != nullptr)
    {
        ::chdir(oldPwd);
        ::free(oldPwd);
    }
    return pass;
}

int CTestReader::TestCommand()
{
    return TestRun(test_command);
}

/* ---------------------------------------------------------------------- */

class CTestWriter::Impl
{
public:
    Impl(const char* outputFile);
    ~Impl();

    void WriteTest(const CTestData& test, const std::string* output = nullptr);
    void Close();

private:
    std::string m_fileName;
    std::ofstream m_stream;
};

CTestWriter::Impl::Impl(const char* outputFile)
    : m_fileName(outputFile), m_stream(outputFile)
{
    m_stream << "<xml>\n";
}

CTestWriter::Impl::~Impl()
{
    if (m_stream.is_open())
    {
        Close();
    }
}

void CTestWriter::Impl::WriteTest(const CTestData& test, const std::string* output)
{
    m_stream << "  <testcase id=\""
        << test.id << "\" description=\""
        << test.description << "\">\n";

    if (test.command != nullptr)
    {
        m_stream << "    <command";
        if (test.command->exitCode != 0)
        {
            m_stream << " exitCode=\"" << test.command->exitCode << "\"";
        }
        if (!test.command->workDir.empty())
        {
            m_stream << " workDir=\"" << test.command->workDir << "\"";
        }
        if (!test.command->match.empty())
        {
            m_stream << " match=\"" << test.command->match << "\"";
        }
        m_stream << ">\n";
        if (!test.command->cmdLine.empty())
        {
            m_stream << test.command->cmdLine << "\n";
        }
        m_stream << "    </command>\n";
    }

    if (!test.input.empty())
    {
        m_stream << "    <input>\n";
        m_stream << test.input << "\n";
        m_stream << "    </input>\n";
    }

    if (!test.data.empty())
    {
        m_stream << "    <data>\n";
        m_stream << test.data << "\n";
        m_stream << "    </data>\n";
    }

    if (!test.expect.empty())
    {
        m_stream << "    <expect>\n";
        m_stream << test.expect << "\n";
        m_stream << "    </expect>\n";
    }

    if (output != nullptr && !output->empty())
    {
        m_stream << "    <output>\n";
        m_stream << *output << "\n";
        m_stream << "    </output>\n";
    }

    m_stream << "  </testcase>\n";
}

void CTestWriter::Impl::Close()
{
    m_stream << "</xml>\n";
    m_stream.close();
}

/* ---------------------------------------------------------------------- */

CTestWriter::CTestWriter(const char* outputFile) : m_pImpl(new Impl(outputFile))
{
}

CTestWriter::~CTestWriter()
{
    if (m_pImpl != nullptr)
    {
        delete m_pImpl;
        m_pImpl = nullptr;
    }
}

void CTestWriter::WriteTest(const CTestData& test, const std::string* output)
{
    m_pImpl->WriteTest(test, output);
}

void CTestWriter::Close()
{
    m_pImpl->Close();
}

}
} /* test::xml */ 

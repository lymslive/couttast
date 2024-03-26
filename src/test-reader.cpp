#include "test-reader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace tast
{
namespace util
{

CTestReader::CTestReader(const char* inputFile)
    : m_fileName(inputFile), m_stream(inputFile)
{
}

bool CTestReader::NextTest(CTestData& test)
{
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

bool CTestReader::ExtractTest(CTestData& test, std::stringstream& stream)
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
        }
    }

    return found;
}

std::string CTestReader::ExtractAttribute(const std::string& line, const std::string& attr)
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

std::string CTestReader::ExtractContent(std::stringstream& stream, const std::string& tag)
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

CTestWriter::CTestWriter(const char* outputFile)
    : m_fileName(outputFile), m_stream(outputFile)
{
    m_stream << "<xml>\n";
}

CTestWriter::~CTestWriter()
{
    if (m_stream.is_open())
    {
        Close();
    }
}

void CTestWriter::WriteTest(const CTestData& test, const std::string* output)
{
    m_stream << "  <testcase id=\""
        << test.id << "\" description=\""
        << test.description << "\">\n";

    m_stream << "    <input>\n"
        << test.input << "\n"
        << "    </input>\n";

    m_stream << "    <data>\n"
        << test.data << "\n"
        << "    </data>\n";

    m_stream << "    <expect>\n"
        << test.expect << "\n"
        << "    </expect>\n";

    if (output != nullptr)
    {
        m_stream << "    <output>\n"
            << *output << "\n"
            << "    </output>\n";
    }

    m_stream << "  </testcase>\n";
}

void CTestWriter::Close()
{
    m_stream << "</xml>\n";
    m_stream.close();
}

}
} /* test::util */ 

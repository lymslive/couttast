/**
 * @file test-reader.h
 * @author lymslive
 * @date 2024-03-25
 * @brief Read a list of test case data for a simple xml file.
 * */
#pragma once
#ifndef TEST_READER_H__
#define TEST_READER_H__

#include <string>
#include <fstream>

/* A simple xml format with a list of test case data.
 *
 * <xml>
 *   <testcase id="a_word" description="a not long string">
 *     <input>
 *       multiple lines exclude the leading and tailing "\n".
 *     </input>
 *     <data>
 *       multiple lines exclude the leading and tailing "\n".
 *     </data>
 *     <expect>
 *       multiple lines exclude the leading and tailing "\n".
 *     </expect>
 *   </testcase>
 * </xml>
 */

namespace tast
{
namespace util
{
/// Represent a test case, contains each necessary part.
struct CTestData
{
    std::string id;
    std::string description;
    std::string input;
    std::string data;
    std::string expect;
};

/// Parser reader for a file with a list of test case.
class CTestReader
{
public:
    /// Constructor from an input file name.
    CTestReader(const char* inputFile);

    /// Get the next test case, return false when reach EOF or error.
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

/// Write back a list test case to another xml file.
class CTestWriter
{
public:
    /// Constructor from an output file name.
    CTestWriter(const char* outputFile);
    ~CTestWriter();

    /// Write a test case along with it real output in runtime to file stream.
    /// Add a <ouput> element after the <expect> if pass the second argument.
    void WriteTest(const CTestData& test, const std::string* output = nullptr);

    /// Close write file after write close </xml>
    void Close();

private:
    std::string m_fileName;
    std::ofstream m_stream;
};

}
} /* test::util:: */ 

#endif /* end of include guard: TEST_READER_H__ */


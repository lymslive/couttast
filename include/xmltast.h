/**
 * @file xmltast.h
 * @author lymslive
 * @date 2024-04-12 2024-04-15
 * @brief Read and deal with a list of test case data from a simple xml file.
 * */
#pragma once
#ifndef XML_TAST_H__
#define XML_TAST_H__

#include <string>
#include <fstream>
#include <functional>

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
 *     <command exitCode="0" workDir="/new/path" match="trim">
 *       single or multi line for a shell command.
 *     </command>
 *     <expect>
 *       multiple lines exclude the leading and tailing "\n".
 *     </expect>
 *   </testcase>
 * </xml>
 */

namespace tast
{
namespace xml
{

/// Special extern command test data.
struct CTestCommand
{
    /// The command to run, which is required.
    std::string cmdLine;

    /// Optional exit code, default expect to 0.
    int exitCode = 0;

    /// Optional change to another working directory before run command, 
    /// would change back to the previous working directory after run.
    std::string workDir;

    /// The match flag pass to COUT_SYSTEM macro, refer to EMatchFlag.
    /// Valid value: trim equal substr wild regexp, and default to trim.
    std::string match;
};

/// Represent a test case, each field is from the xml element.
struct CTestData
{
    std::string id;
    std::string description;
    std::string input;
    std::string data;
    std::string expect;
    CTestCommand* command = nullptr;

    ~CTestData();
    void clear();
};

/// Function to deal with one test case data, return true if passed.
/// In the function may use COUT macro to assert the test.
/// Argument `CTestData` is mutable, can directly consume the content for efficiency.
typedef std::function<bool(CTestData& test)> FnTest;

/// Parser reader for a file with a list of test case.
class CTestReader
{
public:
    /// Constructor from an input file name.
    CTestReader(const char* inputFile);
    ~CTestReader();

    /// Get the next test case, return false when reach EOF or error.
    /// The input argument `test` would reset to read fresh data.
    bool NextTest(CTestData& test);

    /// Test the only cases contains command element.
    int TestCommand();

    /// Test case with user provided function.
    int TestRun(FnTest fn);

private:
    class Impl;
    Impl* m_pImpl = nullptr;
};    

/// Write back a list test case to another xml file.
class CTestWriter
{
public:
    /// Constructor from an output file name.
    CTestWriter(const char* outputFile);
    ~CTestWriter();

    /// Write a test case along with it real output in runtime to the file.
    /// Add a <ouput> element after the <expect> if pass the second argument.
    void WriteTest(const CTestData& test, const std::string* output = nullptr);

    /// Close write file after writing the closed tag </xml>.
    void Close();

private:
    class Impl;
    Impl* m_pImpl = nullptr;
};

}
} /* test::xml:: */ 

#endif /* end of include guard: XML_TAST_H__ */


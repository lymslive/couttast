/** 
 * @file format.h 
 * @author lymslive
 * @date 2023-06-26
 * @brief Structs used in this extend unit test library.
 * */
#ifndef FORMAT_H__
#define FORMAT_H__

#include "tinyini.h"

#include <string>
#include <vector>

namespace tast
{

/// Type of container for test case.
typedef const CTastCase* TastEntry;
typedef std::vector<TastEntry> TastList;
typedef std::vector<CTastCase> TastPool;

/// Make a vector of pointer from vector of tast case object.
TastList MakeTastList(const TastPool& tastPool);

/// Find a test case by exact name, return nullptr if not found.
TastEntry FindTastCase(const TastList& tastList, const std::string& name);

/// Supported config information from cli or ini.
struct CTastConfig
{
    char help = 0;   //< --help flag
    char list = 0;   //< --list or --List flag
    char colour = 0; //< enable colour print or not
    char job = -1;   //< --job= option, multi-process
    bool all = false;    //< --all flag, include tools
    bool notool = false; //< --notool flag, exclude tools
    bool random = false; //< --random flag, reorder filtered cases
    std::string cout;    //< --cout= option, mask control print
    std::vector<std::string> adds; //< position argument may prefix +
    std::vector<std::string> subs; //< single - option to sub test cases

    void ParseCli(const CTinyCli& cli);
};

struct CTastStatement
{
    bool ok = true;
    std::string expression;
    std::string valueEvaluate;
    std::string valueExpected;
    std::string location;
};

/// Message after each test case, and also summary count.
/// << [PASS | FAIL] {count} {name} {runtime} us.
/// In summary count, only first two fields.
struct CTastFoot
{
    bool failed = false; //< indicate the case is failed or not.
    int count = 0;       //< the count of failed statements.
    std::string name;    //< test case name
    int64_t runtime = 0; //< run time in micro-seconds

    /// Parse from formated line text, return parsed field count.
    /// Summary foot return 2, case foot return 4.
    /// Error return <= 0.
    int ReadLine(const char* psz, size_t len);

    int ReadLine(const std::string& line)
    {
        return ReadLine(line.c_str(), line.size());
    }

};

/// Message summary after run all planed test case.
/// Multi-lines to report passed count and failed count, then list failed
/// names of test case.
struct CTastSummary
{
    int fail = 0;
    int pass = 0;
    std::vector<std::string> failNames;

    void Print(std::string& output) const;
};

} // end of namespace tast
#endif /* end of include guard: FORMAT_H__ */

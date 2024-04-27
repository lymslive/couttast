// A very tiny unit test framework with single header even in C++98
// lymslive @ 2023-06-17 2024-04-17
#ifndef TINYTAST_HPP__
#define TINYTAST_HPP__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

namespace tast
{

inline int64_t GetMicrosecondTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((int64_t) tv.tv_sec * 1000000 + tv.tv_usec);
}

/// A min command line parser, only support `--option[=val]` except arguments
struct CTinyCli
{
    std::map<std::string, std::string> m_mapOption;
    std::vector<std::string> m_vecArg;

    void ParseCli(int argc, char** argv)
    {
        for (int i = 1; i < argc; ++i)
        {
            if (!argv[i] || argv[i][0] == '\0')
            {
                continue;
            }

            // prefix --
            const char* k = argv[i];
            while (*k == '-')
            {
                ++k;
            }

            // midfix =
            const char* v = NULL;
            for (const char* s = k; *s != '\0'; ++s)
            {
                if (*s == '=')
                {
                    v = ++s;
                    break;
                }
            }

            if (v != NULL) // && *v != '\0', accept empty val
            {
                // key=val --key=val --key=
                std::string val = v;
                std::string key(k, v-1);
                m_mapOption[key] = val;
            }
            else if (k != argv[i] && *k != '\0')
            {
                // --key, save it's val as --
                std::string key(k);
                m_mapOption[key] = std::string(int(k - argv[i]), '-');
            }
            else
            {
                // normal arg or only --
                m_vecArg.push_back(argv[i]);
            }
        }
    }
};

const int MAX_PRINT_BUFFER = 1024;

// control output when run tast
const int COUT_BIT_COUT = 1;      //< output non-compare statement
const int COUT_BIT_PASS = 1 << 1; //< output passed statement
const int COUT_BIT_FAIL = 1 << 2; //< output failed statement
const int COUT_BIT_DESC = 1 << 3; //< output DESC macro for context line
const int COUT_BIT_LINE = 1 << 4; //< output an empty line after each test case
const int COUT_BIT_HEAD = 1 << 5; //< output head line for a test case
const int COUT_BIT_FOOT = 1 << 6; //< output foot line for a test case
const int COUT_BIT_LAST = 1 << 7; //< output summay after all test case

// alias for common maks with conbination of cout bits
const int COUT_MASK_NONE = 0;
const int COUT_MASK_ALL = 0xFFFF;
const int COUT_MASK_FAIL = COUT_MASK_ALL & (~COUT_BIT_PASS);
const int COUT_MASK_SILENT = COUT_BIT_FAIL | COUT_BIT_FOOT | COUT_BIT_LAST;

/// A test function type without any argument nor return value.
typedef void (*voidfun_t)();

/// Abstract base class for a test case.
struct CTastCase
{
    voidfun_t   m_run;         //< the function to run test.
    const char* m_name;        //< the name of test case.
    const char* m_description; //< one line description for test case.
    const char* m_file;        //< file name where defined the test case.
    uint16_t m_line;           //< line number within file where defined.
    uint8_t  m_nameLen;        //< the length of name, max 256.
    uint8_t  m_fileLen;        //< the length of file, max 256.
    bool m_autoRun;            //< can auto run without cli argument.

    CTastCase() {}
    CTastCase(voidfun_t run, const char* name, const char* desc, const char* file, int line, bool autoRun = true)
        : m_run(run), m_name(name), m_description(desc), m_file(file), m_line(line), m_autoRun(autoRun)
    {
        m_nameLen = ::strlen(name);
        m_fileLen = ::strlen(file);
    }

    bool EqualName(const std::string& name) const
    {
        return m_nameLen == name.size() && ::memcmp(m_name, name.c_str(), name.size()) == 0;
    }

    const char* MatchName(const std::string& name) const
    {
        if (m_nameLen < name.size())
        {
            return NULL;
        }
        return ::strstr(m_name, name.c_str());
    }

    bool MatchFile(const std::string& file, int line = 0) const
    {
        return m_fileLen >= file.size() && ::strstr(m_file, file.c_str()) != NULL && m_line >= line;
    }

    std::string List(bool full = false) const
    {
        std::string line(m_name);
        if (!full)
        {
            line.append(m_autoRun ? "" : "*");
        }
        else
        {
            char lineno[16] = {0};
            snprintf(lineno, sizeof(lineno), "%d", m_line);
            line.append("\t").append(m_autoRun ? "TAST": "TOOL")
                .append("\t").append(m_file).append("\t").append(lineno)
                .append("\t").append(m_description);
        }
        return line;
    }
};

/// Manage the collection of test cases.
class CTastMgr : public CTinyCli
{
    std::vector<CTastCase> m_tastPool; //< all test case
    std::vector<const char*> m_listFail;   //< failed test case name
    int64_t m_beginTime; //< case begin time in microsecond
    int64_t m_endTime;   //< case end time in microsecond
    int m_currentFail;   //< count of failed statement in current test case
    int m_passedCase;    //< count of passed test case
    int m_failedCase;    //< count of failed test case
    int m_coutMask;      //< bit mask to control output

    typedef void (*PrintFun)(const char* str);
    PrintFun m_fnPrint;  //< custome print function for output
    const CTastCase* m_tastRun; //< the current running test case

    friend class CStatement;
    friend class CTastBuilder;
public: // constructor
    static CTastMgr* GetInstance()
    {
        static CTastMgr instance;
        return &instance;
    }

    CTastMgr() : m_beginTime(0), m_endTime(0),
        m_currentFail(0), m_passedCase(0), m_failedCase(0),
        m_coutMask(COUT_MASK_ALL), m_fnPrint(NULL), m_tastRun(NULL)
    {
        m_tastPool.reserve(128);
    }

    const std::vector<CTastCase>& GetTastPool() const { return m_tastPool; }
    const CTastCase* GetTastRun() const { return m_tastRun; }

public: // output
    void CoutEnable (int bit) { m_coutMask = m_coutMask | bit; }
    void CoutDisable (int bit) { m_coutMask = m_coutMask & (~bit); }
    bool CoutMask(int bit) const { return (m_coutMask & bit) != 0; }
    void SetCoutMask(int bit) { m_coutMask = bit; }
    void SetCoutMask(const std::string& mask)
    {
        if (mask == "fail")
        {
            m_coutMask = COUT_MASK_FAIL;
        }
        else if (mask == "silent")
        {
            m_coutMask = COUT_MASK_SILENT;
        }
        else if (mask == "none")
        {
            m_coutMask = COUT_MASK_NONE;
        }
    }

    void SetPrint(PrintFun fn) { m_fnPrint = fn; }
    void Print(const char* str) const
    {
        if (m_fnPrint != NULL)
        {
            m_fnPrint(str);
        }
        else
        {
            printf("%s\n", str);
            fflush(stdout);
        }
    }

    void Print(int bit, const char* format, ...) const
    {
        if (bit != 0 && !CoutMask(bit)) { return; }
        static char buffer[MAX_PRINT_BUFFER] = {0};
        va_list vlist;
        va_start(vlist, format);
        vsnprintf(buffer, sizeof(buffer), format, vlist);
        va_end(vlist);
        return Print(buffer);
    }

public: // how to run tast

    void AddTast(const CTastCase& instance)
    {
        m_tastPool.push_back(instance);
    }

    static const char* pass_case(bool tf)
    {
        return tf ? "PASS" : "FAIL";
    }

    void SetBeginTime()
    {
        m_beginTime = GetMicrosecondTime();
        m_endTime = 0;
    }
    int64_t SetEndTime()
    {
        if (m_endTime <= 0)
        {
            m_endTime = GetMicrosecondTime();
        }
        return m_endTime - m_beginTime;
    }

    void PreRunTast(const char* name)
    {
        m_currentFail = 0;
        Print(COUT_BIT_HEAD, "## run %s()", name);
        SetBeginTime();
    }

    void PostRunTast(const char* name)
    {
        int64_t passTime = SetEndTime();

        if (m_currentFail == 0)
        {
            m_passedCase++;
        }
        else
        {
            m_failedCase++;
            m_listFail.push_back(name);
        }

        Print(COUT_BIT_FOOT, "<< [%s] %d %s %lld us", pass_case(m_currentFail == 0), m_currentFail, name, passTime);
        Print(COUT_BIT_LINE, "");
    }

    int Summary() const
    {
        Print(COUT_BIT_LAST, "## Summary\n<< [%s] %d\n<< [%s] %d", pass_case(true), m_passedCase, pass_case(false), m_failedCase);
        for (size_t i = 0; i < m_listFail.size(); ++i)
        {
            Print(COUT_BIT_LAST, "!! %s", m_listFail[i]);
        }
        return m_failedCase;
    }

    void RunTast(const CTastCase& tast, bool bOnlyAuto = false)
    {
        if (bOnlyAuto && !tast.m_autoRun) { return; }

        PreRunTast(tast.m_name);
        m_tastRun = &tast;
        tast.m_run();
        PostRunTast(tast.m_name);
    }

    CTastCase* FindTastCase(const std::string& name)
    {
        for (std::vector<CTastCase>::iterator it = m_tastPool.begin(); it != m_tastPool.end(); ++it)
        {
            if (it->EqualName(name))
            {
                return &(*it);
            }
        }
        return NULL;
    }

    int64_t TimeTast(voidfun_t run, int times = 10, int msleep = 1000)
    {
        int64_t beginTimeSave = m_beginTime;
        int64_t endTimeSave = m_endTime;
        int64_t timeTotal = 0;
        for (int i = 0; i < times; ++i)
        {
            SetBeginTime();
            run();
            int64_t passTime = SetEndTime();
            timeTotal += passTime;
            if (msleep > 0)
            {
                Print(COUT_BIT_HEAD, ".. time %d: %lld us", i, passTime);
                ::usleep(msleep * 1000);
            }
        }
        m_beginTime = beginTimeSave;
        m_endTime = endTimeSave;
        int64_t timeAvg = times > 0 ? timeTotal/times : 0;
        Print(COUT_BIT_FOOT, ".. time: %d; average %lld us; total: %lld us", times, timeAvg, timeTotal);
        return timeAvg;
    }

    int64_t TimeTast(const std::string& name, int times = 10, int msleep = 1000)
    {
        CTastCase* pTastCase = FindTastCase(name);
        if (pTastCase == NULL) { return 0; }
        Print(COUT_BIT_HEAD, "## time %s() with %d runs", name.c_str(), times);
        return TimeTast(pTastCase->m_run, times, msleep);
    }

public: // what tast to run

    // run test cases defined in or match `file` after `line`.
    void RunTast(const std::string& file, int line)
    {
        for (std::vector<CTastCase>::iterator it = m_tastPool.begin(); it != m_tastPool.end(); ++it)
        {
            if (it->MatchFile(file, line))
            {
                RunTast(*it, true);
            }
        }
    }

    // find tast by name to run, or `file.cpp:line` form, NOT support wild char [^$*].
    void RunTast(const std::string& name)
    {
        if (name.empty()) { return; }

        size_t idot = name.rfind(".cpp");
        if (idot != std::string::npos)
        {
            std::string file = name.substr(0, idot + 4);
            int line = 0;
            size_t icolon = name.rfind(":");
            if (icolon != std::string::npos)
            {
                line = atoi(name.c_str() + icolon + 1);
            }
            return RunTast(file, line);
        }

        std::vector<CTastCase*> filter;
        for (std::vector<CTastCase>::iterator it = m_tastPool.begin(); it != m_tastPool.end(); ++it)
        {
            if (it->EqualName(name))
            {
                return RunTast(*it);
            }
            if (it->MatchName(name))
            {
                filter.push_back(&(*it));
            }
        }

        for (std::vector<CTastCase*>::iterator it = filter.begin(); it != filter.end(); ++it)
        {
            RunTast(*(*it));
        }
    }

    // select tast to run after parsed cli arguments.
    int RunTast()
    {
        for (std::map<std::string, std::string>::iterator it = m_mapOption.begin(); it != m_mapOption.end(); ++it)
        {
            if (it->first == "help") return ListHelp();
            if (it->first == "list") return ListCase(false);
            if (it->first == "List") return ListCase(true);
            if (it->first == "cout") { SetCoutMask(it->second); }
        }

        if (!m_vecArg.empty())
        {
            for (size_t i = 0; i < m_vecArg.size(); ++i)
            {
                RunTast(m_vecArg[i]);
            }
        }
        else
        {
            for (std::vector<CTastCase>::iterator it = m_tastPool.begin(); it != m_tastPool.end(); ++it)
            {
                RunTast(*it, true);
            }
        }
        return Summary();
    }

    // run tast with argc and argv from main function.
    int RunTast(int argc, char** argv)
    {
        ParseCli(argc, argv);
        return RunTast();
    }

public: // overview tast
    int ListCase(bool bTable) // no const as below c++11 there in no cbegin()
    {
        for (std::vector<CTastCase>::iterator it = m_tastPool.begin(); it != m_tastPool.end(); ++it)
        {
            Print(it->List(bTable).c_str());
        }
        return 0;
    }

    int ListHelp() // const
    {
        Print("./tast_program [options] [testcase ...]");
        Print("  --list (--List): list all test cases (also with description)");
        Print("  --cout=[fail|silent|none]: control less print output");
        Print("  --help: print this message");
        return 0;
    }
};

/// Build a concrete test case instance.
struct CTastBuilder
{
    CTastBuilder(voidfun_t run, const char* name, const char* file, int line, bool autoRun, const char* desc = "")
    {
        const char* slash = strrchr(file, '/');
        file = slash ? slash + 1 : file;
#if __cplusplus >= 201103L
        CTastMgr::GetInstance()->m_tastPool.emplace_back(run, name, desc, file, line, autoRun);
#else
        CTastCase instance(run, name, desc, file, line, autoRun);
        CTastMgr::GetInstance()->AddTast(instance);
#endif
    }
};

/// The source location of a statement.
struct CLocation
{
    const char* function; //< from __FUNCTION__
    const char* file;     //< from __FILE__
    int line;             //< from __LINE__

    CLocation(const char* pFile, int iLine, const char* pFunction)
        : file(pFile), line(iLine), function(pFunction)
    {}
};

/// Denote a COUT statement or like.
struct CStatement
{
    CLocation m_stLocation;    //< source location of this statement.
    const char* w_pExpression; //< the expression in literal string.
    CTastMgr* w_pTastMgr;      //< refer to the manager of test.
    bool m_bCoutFailOnly;      //< only output when fail for COUTF.

    CStatement(const CLocation& stLocation, const char* pExpression, bool bCoutFailOnly = false)
        : m_stLocation(stLocation), w_pExpression(pExpression), m_bCoutFailOnly(bCoutFailOnly)
    {
        w_pTastMgr = CTastMgr::GetInstance();
    }

    CStatement& SetTastMgr(CTastMgr& stTastMgr)
    {
        w_pTastMgr = &stTastMgr;
        return *this;
    }

    static const char* pass_cout(bool tf)
    {
        return tf ? "OK" : "NO";
    }

    std::string cout_value(bool tf)
    {
        return tf ? "ture" : "false";
    }

    template <typename T>
    std::string cout_value(const T& value)
    {
        std::stringstream oss;
        oss << value;
        return oss.str();
    }

    template <typename T>
    bool cout(const T& valExpr)
    {
        w_pTastMgr->Print(COUT_BIT_COUT, "|| %s =~? %s", w_pExpression, cout_value(valExpr).c_str());
        return true;
    }

    template <typename U, typename V>
    bool cout(const U& valExpr, const V& valExpect, bool bPass, const char* pExpression = NULL)
    {
        if (pExpression != NULL)
        {
            w_pExpression = pExpression;
        }
        if (!bPass)
        {
            w_pTastMgr->m_currentFail++;
        }
        if (bPass && !m_bCoutFailOnly)
        {
            w_pTastMgr->Print(COUT_BIT_PASS, "|| %s =~? %s [%s]", w_pExpression, cout_value(valExpr).c_str(), pass_cout(bPass));
        }
        else if (!bPass)
        {
            w_pTastMgr->Print(COUT_BIT_FAIL, "|| %s =~? %s [%s]", w_pExpression, cout_value(valExpr).c_str(), pass_cout(bPass));
            w_pTastMgr->Print(COUT_BIT_FAIL, ">> Expect: %s", cout_value(valExpect).c_str());
            w_pTastMgr->Print(COUT_BIT_FAIL, ">> Location: [%s:%d](%s)", m_stLocation.file, m_stLocation.line, m_stLocation.function);
        }
        return bPass;
    }

    template <typename U, typename V>
    bool cout(const U& valExpr, const V& valExpect)
    {
        return cout(valExpr, valExpect, valExpr == valExpect);
    }

    bool cout(double valExpr, double valExpect)
    {
        // compare double a == b as much precision as possible.
        bool bPass = (valExpr >= valExpect) && (valExpr <= valExpect);
        return cout(valExpr, valExpect, bPass);
    }

    bool cout(double valExpr, double valExpect, double limit)
    {
        bool bPass = (valExpr > valExpect) ? ((valExpr - valExpect) <= limit) : ((valExpect - valExpr) <= limit);
        return cout(valExpr, valExpect, bPass);
    }

    void cout_error(int count)
    {
        if (cout(w_pTastMgr->m_currentFail, count))
        {
            w_pTastMgr->m_currentFail = 0;
        }
    }
};

} // end of namespace tast

/// The global tast manager instance object.
#define G_TASTMGR tast::CTastMgr::GetInstance()

/// Run all auto tests or those specified in cli argument.
#define RUN_TAST(...) G_TASTMGR->RunTast(__VA_ARGS__)

/// Define test case, with static builder adding to global manager before main.
/// The following code bock is the body of test function.
#define TAST_RUNNER(name) tast_##name##_runner
#define TAST_BUILDER(name) tast_##name##_builder
#define DEF_TAST_IMPL(name, autoRun, ...) \
    static void TAST_RUNNER(name)(); \
    static tast::CTastBuilder TAST_BUILDER(name)(TAST_RUNNER(name), #name, __FILE__, __LINE__, autoRun, ## __VA_ARGS__); \
    void  TAST_RUNNER(name)()

/// The TAST case by can be auto run without argument.
#define DEF_TAST(name, ...) DEF_TAST_IMPL(name, true, ## __VA_ARGS__)
/// The TOOL case need explicit argument to run.
#define DEF_TOOL(name, ...) DEF_TAST_IMPL(name, false, ## __VA_ARGS__)

/// Location used in other statement macros.
#define SRC_LOCATION tast::CLocation(__FILE__, __LINE__, __FUNCTION__)

/// Print the result of `expr`, may compare with `expect` to determine fail or pass.
#define COUT(expr, ...) tast::CStatement(SRC_LOCATION, #expr).cout((expr), ## __VA_ARGS__)

/// Only print the result if fail to compare `expr` with `expect`.
#define COUTF(expr, expect, ...) tast::CStatement(SRC_LOCATION, #expr, true).cout((expr), (expect), ## __VA_ARGS__)

/// Early return the current test function if the compare fails.
#define COUT_ASSERT(expr, expect, ...) if (!tast::CStatement(SRC_LOCATION, #expr).cout((expr), (expect), ## __VA_ARGS__)) return

/// Print description message for some test code.
#define DESC(msg, ...) G_TASTMGR->Print(tast::COUT_BIT_DESC, "-- " msg, ## __VA_ARGS__)

/// Set the begin time for measurement range within a test case.
/// Otherwise default report the elapsed time(us) to run the whole case.
#define TIME_TIC G_TASTMGR->SetBeginTime()
/// Set the end time and return the elapsed time(us) since `TIC`.
#define TIME_TOC G_TASTMGR->SetEndTime()

/// Run named tast or void function several times, return the average elapse microsecond. 
/// 1-3 arguemnt: TIME_TAST(name, times=10, msleep=1000)
/// The 3rd argument means sleep several milli-second after each run and print
/// some verbose message, pass 0 to suppress this behavior.
/// In the target tast or void function, may use TIME_TIC or(and) TIM_TOC to
/// reduce the measured time range.
#define TIME_TAST(...) G_TASTMGR->TimeTast(__VA_ARGS__)

/// Access cli options stored in a map, may useful in body of `DEF_TOOL`. 
/// Note `TAST_OPTION[key]` may insert with empty value when key not exist.
#define TAST_OPTION G_TASTMGR->m_mapOption

/// Conditional compile macro for unit test program.
#define HAS_UNIT_TEST 1

#endif /* end of include guard: TINYTAST_HPP__ */

// A very tiny unit test framework with single header even in C++98
// lymslive @ 2023-06-17
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

struct CTastMgr; // forward declaration

/// Abstract base class for a test case.
struct CTastCase
{
    CTastMgr* w_pTastMgr;      //< the owner `CTastMgr` object.
    std::string m_description; //< one line description for test case.
    std::string m_file;        //< file name where defined the test case.
    int m_line;                //< line number within file where defined.
    bool m_autoRun;            //< can auto run without cli argument.

    CTastCase(CTastMgr* pTastMgr, const std::string& desc, const std::string& file, int line, bool autoRun = true)
        : w_pTastMgr(pTastMgr), m_description(desc), m_file(file), m_line(line), m_autoRun(autoRun) {}
    virtual ~CTastCase() {}
    virtual void run() = 0;

    std::string List(const std::string& prefix, bool full = false)
    {
        std::string line(prefix);
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
    std::map<std::string, CTastCase*> m_mapTastCase; // all test case by name
    std::vector<std::string> m_listFail; //< failed test case name
    int64_t m_beginTime; //< case begin time in microsecond
    int64_t m_endTime;   //< case end time in microsecond
    int m_currentFail;   //< count of failed statement in current test case
    int m_passedCase;    //< count of passed test case
    int m_failedCase;    //< count of failed test case
    int m_coutMask;      //< bit mask to control output

    typedef void (*PrintFun)(const char* str);
    PrintFun m_fnPrint;  //< custome print function for output

    friend class CStatement;
public: // constructor
    static CTastMgr* GetInstance()
    {
        static CTastMgr instance;
        return &instance;
    }

    CTastMgr() : m_beginTime(0), m_endTime(0),
        m_currentFail(0), m_passedCase(0), m_failedCase(0),
        m_coutMask(COUT_MASK_ALL), m_fnPrint(NULL)
    {}

    ~CTastMgr()
    {
        for (std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.begin(); it != m_mapTastCase.end(); ++it)
        {
            if (it->second)
            {
                delete it->second;
                it->second = NULL;
            }
        }
    }

    const std::map<std::string, CTastCase*>& GetTastCase() const
    { return m_mapTastCase; }

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

    void AddTast(const std::string& name, CTastCase* instance)
    {
        if (!instance)
        {
            return;
        }
        if (m_mapTastCase.find(name) == m_mapTastCase.end())
        {
            m_mapTastCase[name] = instance;
        }
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

    void PreRunTast(const std::string& name)
    {
        m_currentFail = 0;
        Print(COUT_BIT_HEAD, "## run %s()", name.c_str());
        SetBeginTime();
    }

    void PostRunTast(const std::string& name)
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

        Print(COUT_BIT_FOOT, "<< [%s] %d %s %lld us", pass_case(m_currentFail == 0), m_currentFail, name.c_str(), passTime);
        Print(COUT_BIT_LINE, "");
    }

    int Summary() const
    {
        Print(COUT_BIT_LAST, "## Summary\n<< [%s] %d\n<< [%s] %d", pass_case(true), m_passedCase, pass_case(false), m_failedCase);
        for (size_t i = 0; i < m_listFail.size(); ++i)
        {
            Print(COUT_BIT_LAST, "!! %s", m_listFail[i].c_str());
        }
        return m_failedCase;
    }

    // run a tast case with name.
    void RunTast(const std::string& name, CTastCase* pTastCase, bool bOnlyAuto = false)
    {
        if (!pTastCase) { return; }
        if (bOnlyAuto && !pTastCase->m_autoRun) { return; }

        PreRunTast(name);
        pTastCase->run();
        PostRunTast(name);
    }

    int64_t TimeTast(const std::string& name, int times = 10, int msleep = 1000)
    {
        std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.find(name);
        if (it == m_mapTastCase.end()) { return 0; }
        Print(COUT_BIT_HEAD, "## time %s() with %d runs", name.c_str(), times);
        int64_t beginTimeSave = m_beginTime;
        int64_t endTimeSave = m_endTime;
        int64_t timeTotal = 0;
        for (int i = 0; i < times; ++i)
        {
            SetBeginTime();
            it->second->run();
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
        Print(COUT_BIT_FOOT, ".. time: %d; average %lld us; totol: %lld us", times, timeAvg, timeTotal);
        return timeAvg;
    }

public: // what tast to run

    // run test cases defined in `file` after `line`.
    void RunTast(const std::string& file, int line)
    {
        for (std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.begin(); it != m_mapTastCase.end(); ++it)
        {
            if (it->second != NULL && it->second->m_file.find(file) != std::string::npos && it->second->m_line >= line)
            {
                RunTast(it->first, it->second);
            }
        }
    }

    // find tast by name to run, or `file.cpp:line` form, NOT support wild char [^$*].
    void RunTast(const std::string& name)
    {
        if (name.empty()) { return; }

        std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.find(name);
        if (it != m_mapTastCase.end())
        {
            return RunTast(name, it->second);
        }
        else
        {
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

            for (std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.begin(); it != m_mapTastCase.end(); ++it)
            {
                if (it->first.find(name) != std::string::npos)
                {
                    RunTast(it->first, it->second);
                }
            }
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
            for (std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.begin(); it != m_mapTastCase.end(); ++it)
            {
                RunTast(it->first, it->second, true);
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
        for (std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.begin(); it != m_mapTastCase.end() && it->second != NULL; ++it)
        {
            Print(it->second->List(it->first, bTable).c_str());
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
template <typename T>
struct CTastBuilder
{
    CTastBuilder(CTastMgr* pTastMgr, const std::string& name, const char* file, int line, bool autoRun, const std::string& desc = "")
    {
        const char* slash = strrchr(file, '/');
        file = slash ? slash + 1 : file;
        CTastMgr::GetInstance()->AddTast(name, new T(pTastMgr, desc, file, line, autoRun));
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

    bool cout(double valExpr, double valExpect, double limit = 0.0001)
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
/// The following code bock is the body virtual function `run()`.
#define DEF_TAST_IMPL(name, autoRun, ...) \
    class CTast_ ## name : public tast::CTastCase \
    { \
    public: \
        CTast_ ## name(tast::CTastMgr* pTastMgr, const std::string& desc, const std::string& file, int line, bool autoArg) : tast::CTastCase(pTastMgr, desc, file, line, autoArg) {} \
        virtual void run(); \
    private: \
        static tast::CTastBuilder<CTast_ ## name> m_builder; \
    }; \
    tast::CTastBuilder<CTast_ ## name> CTast_ ## name::m_builder(G_TASTMGR, #name, __FILE__, __LINE__, autoRun, ## __VA_ARGS__); \
    void CTast_ ## name::run()

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

/// Run a named tast several times and return the average elapse microsecond. 
/// 1-3 arguemnt: TIME_TAST(name, times=10, msleep=1000)
/// The 3rd argument means sleep several milli-second after each run and print
/// some verbose message, pass 0 to suppress this behavior.
#define TIME_TAST(...) G_TASTMGR->TimeTast(__VA_ARGS__)

/// Access cli options stored in a map, may useful in body of `DEF_TOOL`. 
/// Note `TAST_OPTION[key]` may insert with empty value when key not exist.
#define TAST_OPTION G_TASTMGR->m_mapOption

#endif /* end of include guard: TINYTAST_HPP__ */

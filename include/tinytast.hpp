#ifndef TINYTAST_HPP__
#define TINYTAST_HPP__

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

namespace tast
{

template<typename T>
class Singleton
{
public:
    static T* GetInstance()
    {
        static T instance;
        return &instance;
    }

protected:
    Singleton() {}
private:
    Singleton(const T&);
    void operator= (const T&);
};

const int MAX_PRINT_BUFFER = 1024;
inline
void Print(const char* format, ...)
{
    static char buffer[MAX_PRINT_BUFFER] = {0};

    va_list vlist;
    va_start(vlist, format);
    vsnprintf(buffer, sizeof(buffer), format, vlist);
    va_end(vlist);

    printf("%s\n", buffer);
}

inline
const char* pass_str(bool tf, bool eachCout = false)
{
    if (eachCout)
    {
        return tf ? "OK" : "NO";
    }
    return tf ? "PASS" : "FAIL";
}

inline
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
void cout(const char* strExpr, const T& valExpr)
{
    Print("|| %s =~? %s", strExpr, cout_value(valExpr).c_str());
}

class CTinyCli
{
protected:
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

            if (v && *v != '\0')
            {
                // key=val --key==val
                std::string val = v;
                std::string key(k, v-1);
                m_mapOption[key] = val;
            }
            else if (k != argv[i] && *k != '\0')
            {
                // --key
                std::string key(k);
                m_mapOption[key] = "1";
            }
            else
            {
                // normal arg or only --
                m_vecArg.push_back(argv[i]);
            }
        }
    }

    void ParseCli(const std::vector<std::string>& args)
    {
        if (args.empty())
        {
            return;
        }
        std::vector<const char*> argv;
        argv.push_back(""); // argv[0] as program name
        for (size_t i = 0; i < args.size(); ++i)
        {
            argv.push_back(const_cast<char*>(args[i].c_str()));
        }
        return ParseCli((int)argv.size(), const_cast<char**>(&argv[0]));
    }

    std::map<std::string, std::string> m_mapOption;
    std::vector<std::string> m_vecArg;
};

// raw  tast function pointer
typedef void (*PFTAST)();

// tast class
class CTastCase
{
public:
    CTastCase(const std::string& desc) : m_description(desc)
    {}
    virtual ~CTastCase() {}
    virtual void run() = 0;
    const std::string& Description()
    {
        return m_description;
    }
private:
    std::string m_description;
};

class CTastMgr : public Singleton<CTastMgr>, public CTinyCli
{
public:
    CTastMgr()
    {
        m_passedCase = 0;
        m_failedCase = 0;
        m_currentFail = 0;
        m_runMode = NORMAL_RUN;
    }

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

    void AddFail()
    {
        m_currentFail++;
    }

    void PreRunTast(const std::string& name)
    {
        Print("## run %s()", name.c_str());
        m_currentFail = 0;
    }

    void PostRunTast(const std::string& name)
    {
        if (m_currentFail == 0)
        {
            Print("<< [%s] %s", pass_str(true), name.c_str());
            m_passedCase++;
        }
        else
        {
            Print("<< [%s] %d IN %s", pass_str(false), m_currentFail, name.c_str());
            m_failedCase++;
            m_listFail.push_back(name);
        }
        Print("");
    }

    int RunTast(const std::string& name, PFTAST fun)
    {
        PreRunTast(name);
        fun();
        PostRunTast(name);
        return m_currentFail;
    }

    int RunTast(const std::string& name, CTastCase* pTastCase)
    {
        if (!pTastCase)
        {
            return 0;
        }

        if (m_runMode == LIST_NAME)
        {
            m_listTast.push_back(name);
            return 0;
        }
        else if (m_runMode == LIST_WITH_DESC)
        {
            std::string line = name;
            line.append(":\t").append(pTastCase->Description());
            m_listTast.push_back(line);
            return 0;
        }
        else
        {
            PreRunTast(name);
            pTastCase->run();
            PostRunTast(name);
            return m_currentFail;
        }
    }

    int RunTast(const std::string& name)
    {
        if (name.empty())
        {
            return 0;
        }

        std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.find(name);
        if (it != m_mapTastCase.end())
        {
            return RunTast(name, it->second);
        }
        else if (name.size() == 1)
        {
            return RunTast(name, '^');
        }
        else
        {
            size_t iend = name.size() - 1;
            if (name[0] == '^')
            {
                return RunTast(name.substr(1), '^');
            }
            else if (name[iend] == '*')
            {
                return RunTast(name.substr(0, iend), '^');
            }
            else if (name[iend] == '$')
            {
                return RunTast(name.substr(0, iend), '$');
            }
            else if (name[0] == '*')
            {
                return RunTast(name.substr(1), '$');
            }
            else
            {
                return RunTast(name, '%');
            }
        }
    }

    int RunTast(const std::string& arg, char filter)
    {
        int iCount = 0;

        for (std::map<std::string, CTastCase*>::iterator it = m_mapTastCase.begin(); it != m_mapTastCase.end(); ++it)
        {
            const std::string& name = it->first;
            if (arg.size() > name.size())
            {
                continue;
            }

            bool match = false;
            if (filter == '%' && name.find(arg) != std::string::npos)
            {
                match = true;
            }
            else if (filter == '^' && name.find(arg) == 0)
            {
                match = true;
            }
            else if (filter == '$' && name.find(arg) == name.size() - arg.size())
            {
                match = true;
            }

            if (match)
            {
                ++iCount;
                RunTast(name, it->second);
            }
        }

        return iCount;
    }

    int RunTast()
    {
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
                RunTast(it->first, it->second);
            }
        }
        return Summary();
    }

    int RunTast(int argc, char** argv)
    {
        ParseCli(argc, argv);
        return RunTast();
    }

    void ParseCli(int argc, char** argv)
    {
        CTinyCli::ParseCli(argc, argv);
        if (m_mapOption.find("list") != m_mapOption.end())
        {
            m_runMode = LIST_NAME;
        }
        else if (m_mapOption.find("List") != m_mapOption.end())
        {
            m_runMode = LIST_WITH_DESC;
        }
    }

    int Summary()
    {
        if (!m_listTast.empty())
        {
            for (size_t i = 0; i < m_listTast.size(); ++i)
            {
                Print("%s", m_listTast[i].c_str());
            }
            return m_listTast.size();
        }
        else
        {
            Print("## Summary\n");
            Print("<< [%s] %d", pass_str(true), m_passedCase);
            Print("<< [%s] %d", pass_str(false), m_failedCase);
            for (size_t i = 0; i < m_listFail.size(); ++i)
            {
                Print("!! %s", m_listFail[i].c_str());
            }
            return m_failedCase;
        }
    }

    enum ERunMode
    {
        NORMAL_RUN = 0,
        LIST_NAME = 1,
        LIST_WITH_DESC = 2,
    };

private:
    std::map<std::string, CTastCase*> m_mapTastCase;
    // std::string m_currentCase;
    int m_currentFail;
    int m_passedCase;
    int m_failedCase;

    ERunMode m_runMode;
    std::vector<std::string> m_listTast;
    std::vector<std::string> m_listFail;
};

template <typename T>
class CTastBuilder
{
public:
    CTastBuilder(const std::string& name, const std::string& desc = "")
    {
        CTastMgr::GetInstance()->AddTast(name, new T(desc));
    }
};

template <typename U, typename V>
void cout(const char* strExpr, const U& valExpr, const V& valExpect, bool bPass)
{
    const char* strPass = pass_str(bPass, true);
    Print("|| %s =~? %s [%s]", strExpr, cout_value(valExpr).c_str(), strPass);
    if (!bPass)
    {
        Print("!! Expect: %s", cout_value(valExpect).c_str());
        CTastMgr::GetInstance()->AddFail();
    }
}

template <typename U, typename V>
bool compare_value(const U& valExpr, const V& valExpect)
{
    return valExpr == valExpect;
}

template <typename U, typename V>
void cout(const char* strExpr, const U& valExpr, const V& valExpect)
{
    bool bPass = compare_value(valExpr, valExpect);
    return cout(strExpr, valExpr, valExpect, bPass);
}

inline
void cout(const char* strExpr, double valExpr, double valExpect, double limit = 0.0001)
{
    bool bPass = (valExpr > valExpect) ? ((valExpr - valExpect) <= limit) : ((valExpect - valExpr) <= limit);
    return cout(strExpr, valExpr, valExpect, bPass);
}

inline
void run_tast(const char* name, PFTAST fun)
{
    CTastMgr::GetInstance()->RunTast(name, fun);
}

} // end of namespace tast

// satement macros used in tast case function
#define COUT(expr, ...) tast::cout(#expr, (expr), ## __VA_ARGS__)
#define CODE(statement) tast::Print("// %s", #statement); statement
#define DESC(msg, ...) tast::Print("-- " msg, ## __VA_ARGS__)

// macro to invoke simple tast function void()
#define TAST(fun) tast::run_tast(#fun, fun)
#define TAST_RESULT tast::CTastMgr::GetInstance()->Summary()

// macro to define and run tast object
#define RUN_TAST(...) tast::CTastMgr::GetInstance()->RunTast(__VA_ARGS__)
#define DEF_TAST(name, desc) \
    class CTast_ ## name : public tast::CTastCase \
    { \
    public: \
        CTast_ ## name(const std::string& strDesc) : tast::CTastCase(strDesc) {} \
        virtual void run(); \
    private: \
        static tast::CTastBuilder<CTast_ ## name> m_builder; \
    }; \
    tast::CTastBuilder<CTast_ ## name> CTast_ ## name::m_builder(#name, desc); \
    void CTast_ ## name::run()

#endif /* end of include guard: TINYTAST_HPP__ */

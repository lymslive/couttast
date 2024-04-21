#include "couttast.h"
#include "tinyini.h"
#include "agent.h"

#include <string>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>

namespace tast
{

/* ---------------------------------------------------------------------- */

void CheckLeadHome(std::string& src)
{
    if (src.empty() || src[0] != '~')
    {
        return;
    }
    const char* home = ::getenv("HOME");
    if (home != nullptr)
    {
        src.replace(0, 1, home);
    }
}

void CheckChdir(const CTinyIni& cfg)
{
    auto it = cfg.m_mapOption.find("cwd");
    if (it != cfg.m_mapOption.end())
    {
        std::string cwd = it->second;
        CheckLeadHome(cwd);
        if (0 != ::chdir(cwd.c_str()))
        {
            fprintf(stderr, "failed to chdir: %s\n", cwd.c_str());
            ::exit(1);
        }
    }
}

std::string FindConfig(const CTinyIni& cfg)
{
    std::string strIniFile;
    auto it = cfg.m_mapOption.find("config");
    if (it != cfg.m_mapOption.end())
    {
        strIniFile = it->second;
        CheckLeadHome(strIniFile);
        if (0 != ::access(strIniFile.c_str(), R_OK))
        {
            fprintf(stderr, "can not read the specified config file: %s\n", strIniFile.c_str());
            strIniFile.clear();
            ::exit(1);
        }
    }
    else
    {
        std::string strExeName = program_invocation_short_name;
        auto dot = strExeName.find_last_of(".");
        if (dot != 0 && dot != std::string::npos)
        {
            strExeName.erase(dot);
        }
        strIniFile = strExeName + ".ini";
        if (0 != ::access(strIniFile.c_str(), R_OK))
        {
            strIniFile.clear();
        }
    }

    return strIniFile;
}

/* ---------------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    CTinyIni cfg;
    cfg.ParseCli(argc, argv);
    CheckChdir(cfg);

    std::string strIniFile = FindConfig(cfg);
    if (!strIniFile.empty())
    {
        CTinyIni iniTemp;
        if (iniTemp.LoadIni(strIniFile))
        {
            cfg.m_mapOption.emplace("config", strIniFile);
            CheckChdir(iniTemp);
            cfg.Merge(iniTemp);
        }
        else
        {
            fprintf(stderr, "failed to load config: %s\n", strIniFile.c_str());
            strIniFile.clear();
            ::exit(1);
        }
    }

    const char* firstArg = nullptr;
    if (argc > 1)
    {
        firstArg = argv[1];
    }
    return agent_run(*(CTastMgr::GetInstance()), cfg, firstArg);
}

} // end of namespace tast

#if defined(__GNUC__) || defined(__clang__)
    #define WEAK_SYMBOL __attribute__((weak))
#elif defined(_MSC_VER)
    #define WEAK_SYMBOL __declspec(selectany)
#else
    #define WEAK_SYMBOL
#endif

int WEAK_SYMBOL main(int argc, char* argv[])
{
    return tast::main(argc, argv);
}

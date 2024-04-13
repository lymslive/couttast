#include "couttast.h"
#include "tinyini.h"
#include "agent.h"

#include <string>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>

namespace tast
{

bool GetOption(const std::string& key, std::string& val)
{
    return CTinyCliPtr(CTastMgr::GetInstance()).GetValue(val, key);
}

bool GetOption(const std::string& key, int& val)
{
    return CTinyCliPtr(CTastMgr::GetInstance()).GetValue(val, key);
}

const std::vector<std::string>& GetArguments()
{
    return CTastMgr::GetInstance()->m_vecArg;
}

void CheckChdir(const CTinyIni& cfg)
{
    auto it = cfg.m_mapOption.find("cwd");
    if (it != cfg.m_mapOption.end())
    {
        const std::string& cwd = it->second;
        if (0 != ::chdir(cwd.c_str()))
        {
            fprintf(stderr, "failed to chdir: %s\n", cwd.c_str());
        }
    }
}

int main(int argc, char* argv[])
{
    CTinyIni cfg;
    cfg.ParseCli(argc, argv);
    CheckChdir(cfg);

    std::string strExeName = program_invocation_short_name;
    std::string strIniFile = strExeName + ".ini";
    if (0 == access(strIniFile.c_str(), R_OK))
    {
        fprintf(stderr, "load config: %s\n", strIniFile.c_str());
        CTinyIni iniTemp;
        iniTemp.LoadIni(strIniFile);
        CheckChdir(iniTemp);
        cfg.Merge(iniTemp);
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

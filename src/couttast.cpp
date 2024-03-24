/**
 * @file couttast.cpp
 * @author tanshuil
 * @date 2022-06-01
 * @brief 单元测试程序入口通用逻辑处理
 * */
#include "couttast.h"
#include "tinyini.h"
#include "agent.h"

#include <string>
#include <unistd.h>
#include <errno.h>

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

} // end of namespace tast

int weak_main(int argc, char* argv[])
{
    tast::CTinyIni cfg;
    cfg.ParseCli(argc, argv);

    std::string strExeName = program_invocation_short_name;
    std::string strIniFile = strExeName + ".ini";
    if (0 == access(strIniFile.c_str(), R_OK))
    {
        fprintf(stderr, "load config: %s\n", strIniFile.c_str());
        tast::CTinyIni iniTemp;
        iniTemp.LoadIni(strIniFile);
        cfg.Merge(iniTemp);
    }

    if (cfg.m_mapOption.empty() && cfg.m_vecArg.empty())
    {
        // no argument, quick call RUN_TAST
        return tast::CTastMgr::GetInstance()->RunTast();
    }

    return tast::agent_run(*(tast::CTastMgr::GetInstance()), cfg);
}

#define WEAK_SYMBOL __attribute__((weak))
/** 默认的 main() 入口，弱函数，可覆盖。 */
int WEAK_SYMBOL main(int argc, char* argv[])
{
    return weak_main(argc, argv);
}

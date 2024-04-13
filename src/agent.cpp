#include "agent.h"
#include "format.h"
#include "filter.h"
#include "parallel.h"
#include "colour.h"

namespace tast
{

/// Custom warpper for `CTastMgr`, to provide advance features.
struct CTastAgent
{
    CTastMgr* w_pTastMgr = nullptr;    //< Borrowed TastMgr object.
    CTinyIni* w_pConfig = nullptr;     //< Cli arguments merged by ini.
    const char* w_pFirstArg = nullptr; //< First cli argument.
    CTastConfig m_config;              //< Parsed config argument.
    TastList m_vecTastCase;            //< Filtered test case list.

    CTastAgent(CTastMgr& stTastMgr, CTinyIni& cfg, const char* firstArg)
        : w_pTastMgr(&stTastMgr), w_pConfig(&cfg), w_pFirstArg(firstArg)
    {
        m_config.ParseCli(*w_pConfig);
    }

public:
    // Main flow to run test or command.
    int Run();

    /// Print help message for --help.
    bool Help();
    /// Print all or filtered test cases name, one in each line for --list.
    bool List(bool bTable);

    bool SkipRun(int& exitCode)
    {
        return (m_config.help > 0 && Help())
            || (m_config.list > 0 && List(m_config.list == 'L'));
    }

    /// Zero argument run mode. Neither cli nor ini has any arguments.
    bool ZeroMode(int& exitCode);

    /// Run in sub command mode. Cli(but not ini) has argument from DEF_TOOL,
    bool SubCommand(int& exitCode);

    /// Run test cases in current process locally.
    int LocalRun();
    /// Run test cases in multiple child process.
    int ProcessRun()
    {
        return process_run(m_vecTastCase, m_config.job, w_pTastMgr);
    }

    /// Filter test case in `w_pTastMgr`, saved in this `m_mapTastCase`.
    void Filter()
    {
        if (m_vecTastCase.empty())
        {
            filter_tast(w_pTastMgr->GetTastCase(), m_vecTastCase, m_config);
        }
    }

    /// Move cli argument into the tast manager.
    void MoveArgument()
    {
        w_pTastMgr->m_mapOption.swap(w_pConfig->m_mapOption);
        w_pTastMgr->m_vecArg.swap(w_pConfig->m_vecArg);
        if (!m_config.cout.empty())
        {
            w_pTastMgr->SetCoutMask(m_config.cout);
        }
    }

};

/* ---------------------------------------------------------------------- */

bool CTastAgent::Help()
{
    w_pTastMgr->Print("./tast_program [options] [testcase ...]");
    w_pTastMgr->Print("  --list (--List): list all test cases (also with description)");
    w_pTastMgr->Print("  --cout=[fail|silent|none]: control less print output");
    w_pTastMgr->Print("  --all: run all test when no cli argument");
    w_pTastMgr->Print("  --notool: donot run tool even when match cli argument");
    w_pTastMgr->Print("  --random: run or list the test cases in random order");
    w_pTastMgr->Print("  --job=: run in multiple process");
    w_pTastMgr->Print("  --nocolour: disable colour print which default enabled");
    w_pTastMgr->Print("  --colour[=always]: enable colour print even in silent mode or not print to terminal");
    w_pTastMgr->Print("  --cwd: change to new working drirectory");
    w_pTastMgr->Print("  --config=[tast_program.ini]: load the config file, default after the same name as the program");
    w_pTastMgr->Print("  --help: print this message");

    std::string strTast;
    int nTast = 0;
    std::string strTool;
    int nTool = 0;
    for (auto& item : w_pTastMgr->GetTastCase())
    {
        if (item.second->m_autoRun)
        {
            if (nTast++ > 0)
            {
                strTast.append(" ");
            }
            strTast.append(item.first);
        }
        else
        {
            if (nTool++ > 0)
            {
                strTool.append(" ");
            }
            strTool.append(item.first);
        }
    }

    if (nTast > 0)
    {
        w_pTastMgr->Print(0, "tests[%d]: default run without argument\n  %s", nTast, strTast.c_str());
    }
    if (nTool > 0)
    {
        w_pTastMgr->Print(0, "tools[%d]: only run with explicit argument\n  %s", nTool, strTool.c_str());
    }

    return true;
}

bool CTastAgent::List(bool bTable)
{
    if (!m_config.adds.empty() || !m_config.subs.empty() || m_config.random)
    {
        Filter();
    }

    if (!m_vecTastCase.empty())
    {
        for (auto& item : m_vecTastCase)
        {
            if (item.second != nullptr)
            {
                w_pTastMgr->Print(item.second->List(item.first, bTable).c_str());
            }
        }
    }
    else
    {
        for (auto& item : w_pTastMgr->GetTastCase())
        {
            if (item.second != nullptr)
            {
                w_pTastMgr->Print(item.second->List(item.first, bTable).c_str());
            }
        }
    }

	return true;
}

bool CTastAgent::ZeroMode(int& exitCode)
{
    if (w_pFirstArg != nullptr
            || !w_pConfig->m_vecArg.empty() || !w_pConfig->m_mapOption.empty())
    {
        return false;
    }

    if (colour_support())
    {
        w_pTastMgr->SetPrint(colour_print);
    }

    // no argument, quick call RUN_TAST
    exitCode = w_pTastMgr->RunTast();
    return true;
}

bool CTastAgent::SubCommand(int& exitCode)
{
    if (w_pFirstArg == nullptr || w_pFirstArg[0] == '\0'
            || w_pFirstArg[0] == '-' || w_pFirstArg[0] == '+')
    {
        return false;
    }

    auto it = w_pTastMgr->GetTastCase().find(w_pFirstArg);
    if (it == w_pTastMgr->GetTastCase().end())
    {
        return false;
    }

    // not tool
    if (it->second->m_autoRun)
    {
        return false;
    }

    // only help for this tool
    if (m_config.help > 0)
    {
        std::string output;
        it->second->help(output);
        w_pTastMgr->Print(output.c_str());
        return true;;
    }

    MoveArgument();

    // only run this tast
    w_pTastMgr->CoutDisable(COUT_BIT_HEAD);
    w_pTastMgr->CoutDisable(COUT_BIT_FOOT);
    w_pTastMgr->CoutDisable(COUT_BIT_LINE);
    w_pTastMgr->CoutDisable(COUT_BIT_LAST);
    w_pTastMgr->RunTast(it->first, it->second);

    // not set exitCode, let it default to 0.
    return true;
}

int CTastAgent::LocalRun()
{
    for (auto& item : m_vecTastCase)
    {
        w_pTastMgr->RunTast(item.first, item.second);
    }

    int failed = w_pTastMgr->Summary();
    if (failed != 0 && m_config.random)
    {
        std::string order("-- random:");
        for (auto& item : m_vecTastCase)
        {
            order.append(" ").append(item.first);
        }
        if (w_pTastMgr->CoutMask(COUT_BIT_DESC))
        {
            w_pTastMgr->Print(order.c_str());
        }
    }
    return failed;
}

int CTastAgent::Run()
{
    int exitCode = 0;
    if (ZeroMode(exitCode) || SubCommand(exitCode) || SkipRun(exitCode))
    {
        return exitCode;
    }

    Filter();
    MoveArgument();

    if (m_config.colour != 'n' && colour_support() || m_config.colour == 'a')
    {
        w_pTastMgr->SetPrint(colour_print);
    }

    if (m_config.job >= 0 && m_vecTastCase.size() >= MINLIST_TO_PROCESS)
    {
        return ProcessRun();
    }
    else
    {
        return LocalRun();
    }
}

/* ---------------------------------------------------------------------- */

int agent_run(CTastMgr& stTastMgr, CTinyIni& cfg, const char* firstArg)
{
    CTastAgent stAgent(stTastMgr, cfg, firstArg);
    return stAgent.Run();

}

int64_t average_time(voidfun_t fun, int times, int msleep)
{
    if (times <= 0)
    {
        return 0;
    }

    int64_t timeTotal = 0;
    for (int i = 0; i < times; ++i)
    {
        int64_t beginTime = GetMicrosecondTime();
        fun();
        int64_t endTime = GetMicrosecondTime();
        timeTotal += (endTime - beginTime);
        if (msleep > 0)
        {
            ::usleep(msleep * 1000);
        }
    }
    return timeTotal/times;
}

} /* end of tast:: */ 

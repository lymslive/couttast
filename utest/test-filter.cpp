#include "test-agent.h"
#include "filter.h"

std::ostream& operator<<(std::ostream& os, const tast::TastEntry& tast)
{
    os << tast->m_name;
    if (tast->m_autoRun == false)
    {
        os << "*";
    }
    os << "(" << tast->m_file << ":" << tast->m_line<< ")";
    return os;
}

DEF_TAST(filter_basic, "test filter by [+]name and -name")
{
    mock::CTastMgr stTastMgr;
    FillSampleTast(stTastMgr, 20, "ft");
    FillSampleTast(stTastMgr, 20, "mp");
    FillSampleTast(stTastMgr, 20, "sp");
    FillSampleTast(stTastMgr, 20); // "sample"

    DESC("filter: --all");
    {
        tast::CTinyIni cfg;
        cfg.m_mapOption["all"] = "--";
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 80);
        for (auto& item : tastCase)
        {
            std::cout << item << std::endl;
        }
    }

    DESC("filter: no argument");
    {
        tast::CTinyIni cfg;
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 72);
    }

    DESC("filter: +");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("+");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 72);
    }

    DESC("filter: mp");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("mp");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 40);
    }

    DESC("filter: mp +sp");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("mp");
        cfg.m_vecArg.push_back("+sp");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 60);
    }

    DESC("filter: --notool mp +sp");
    {
        tast::CTinyIni cfg;
        cfg.m_mapOption["notool"] = "--";
        cfg.m_vecArg.push_back("mp");
        cfg.m_vecArg.push_back("+sp");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 54);
    }

    DESC("filter: --all -mp -sp");
    {
        tast::CTinyIni cfg;
        cfg.m_mapOption["all"] = "--";
        cfg.m_mapOption["mp"] = "-";
        cfg.m_mapOption["sp"] = "-";
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 20);
    }

    DESC("filter: --all -mp -s");
    {
        tast::CTinyIni cfg;
        cfg.m_mapOption["all"] = "--";
        cfg.m_mapOption["mp"] = "-";
        cfg.m_mapOption["s"] = "-";
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 40);
    }

    DESC("filter: ft-1.cpp # file");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("ft-1.cpp");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 10);
    }

    DESC("filter: ft-1.cpp:50 # file:line");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("ft-1.cpp:50");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 5);
    }

    DESC("filter: f # single char");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("f");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 20);
    }

    DESC("filter: ^f # begin string");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("^f");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 20);
    }

    DESC("filter: ft* # begin string");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("ft*");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 20);
    }

    DESC("filter: 1$ # end string");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("1$");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 8);
    }

    DESC("filter: *1 # end string");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("*1");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 8);
        COUT(tastCase);
    }

    DESC("filter: *1 # end string");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("*11");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 4);
        COUT(tastCase);
    }
}

DEF_TAST(filter_overlap, "test filter when one test name is substr of another")
{
    mock::CTastMgr stTastMgr;
    FillSampleTast(stTastMgr, 20, "mp");
    FillSampleTast(stTastMgr, 20, "longmp");
    FillSampleTast(stTastMgr, 20, "sp");

    DESC("filter: --all");
    {
        tast::CTinyIni cfg;
        cfg.m_mapOption["all"] = "--";
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 60);
        for (auto& item : tastCase)
        {
            std::cout << item << std::endl;
        }
    }

    DESC("filter: +mp_case");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("+mp_case");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 40);
    }

    DESC("filter: +mp_case_10");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("+mp_case_10");
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 1);
        COUT(tastCase);
    }

    DESC("filter: +mp_case_10 --notool");
    {
        tast::CTinyIni cfg;
        cfg.m_vecArg.push_back("+mp_case_10");
        cfg.m_mapOption["notool"] = "--";
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 0);
        COUT(tastCase);
    }
}

DEF_TAST(filter_random, "test filter with --random")
{
    mock::CTastMgr stTastMgr;
    FillSampleTast(stTastMgr, 20);

    tast::TastList tastCaseAll;
    DESC("filter: --all");
    {
        tast::CTinyIni cfg;
        cfg.m_mapOption["all"] = "--";
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 20);
        tastCaseAll.swap(tastCase);
    }

    tast::TastList tastCaseRandom;
    DESC("filter: --random --all");
    {
        tast::CTinyIni cfg;
        cfg.m_mapOption["all"] = "--";
        tast::TastList tastCase;
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCaseAll == tastCase, true);

        cfg.m_mapOption["random"] = "--";
        tastCase.clear();
        tast::filter_tast(stTastMgr.GetTastPool(), tastCase, cfg);
        COUT(tastCase.size(), 20);
        for (auto& item : tastCase)
        {
            std::cout << item << std::endl;
        }
        tastCaseRandom.swap(tastCase);
    }

    if (!COUT(tastCaseAll != tastCaseRandom, true))
    {
        DESC("may randomly equal with litter chance, re-run this test test case to confirm!");
    }
}

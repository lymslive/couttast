#include "test-agent.h"
#include "parallel.h"
#include "../src/parallel.cpp"

DEF_TOOL(pal_single, "run in single-process: compare standar")
{
    mock::CTastMgr stTastMgr;
    FillSampleTast(stTastMgr, 100, "sp");
    stTastMgr.RunTast();
}

// can also use --random
DEF_TOOL(pal_process, "run in multi-process: --process=4 --case=100 --error=0")
{
    int nProcess = 4;
    int nCase = 100;
    int nError = 0;
    tast::GetOption("process", nProcess);
    tast::GetOption("case", nCase);
    tast::GetOption("error", nError);

    DESC("config option: --process=%d, --case=%d", nProcess, nCase);

    srand(123456);
    mock::CTastMgr stTastMgr;
    FillSampleTast(stTastMgr, nCase, "mp");
    if (nError > 0)
    {
        RandError(nError);
    }

    TIME_TIC;
    tast::process_run(stTastMgr.GetTastPool(), nProcess);
    auto toc = TIME_TOC;
    COUT(toc);
    COUT(1.0 * toc / (100 * 100 * 1000 / nProcess), 1.0, 0.01);
}

DEF_TOOL(scanf_foot, "test parse foot string by sscanf")
{
    const char* str = "<< [PASS] filter_basic in 1357 us";
    char pass[8] = {0};
    char name[64] = {0};
    int us = 0;
    int nRead = sscanf(str, "<< [%4s] %s in %d us", pass, name, &us);
    COUT(nRead, 3);
    COUT(pass, std::string("PASS"));
    COUT(name, std::string("filter_basic"));
    COUT(us, 1357);

    str = "<< [FAIL] except_macr in 762 us, 3 errors";
    nRead = sscanf(str, "<< [%4s] %s in %d us", pass, name, &us);
    COUT(nRead, 3);
    COUT(pass, std::string("FAIL"));
    COUT(name, std::string("except_macr"));
    COUT(us, 762);

    str = "<< [fail] Except_Macro ";
    nRead = sscanf(str, "<< [%4s] %s in %d us", pass, name, &us);
    COUT(nRead, 2);
    COUT(pass, std::string("fail"));
    COUT(name, std::string("Except_Macro"));
    COUT(us, 762);
}

DEF_TAST(pal_slice, "test slice_index partition")
{
    DESC("partition: 100 / 4");
    {
        std::vector<tast::IndexRange> ranges = tast::slice_index(100, 4);
        COUT(ranges);
    }

    DESC("partition: 99 / 4");
    {
        std::vector<tast::IndexRange> ranges = tast::slice_index(99, 4);
        COUT(ranges);
    }

    DESC("partition: 101 / 4");
    {
        std::vector<tast::IndexRange> ranges = tast::slice_index(101, 4);
        COUT(ranges);
    }
}

DEF_TAST(pal_runresult, "test deal with runtime result")
{
    const char* sample = "sample.run";
    tast::CRuntimeResult result;
    int nRead = result.ReadFile(sample);
    COUT(nRead, 100);
    COUT(result.m_avgRuntime);

    COUT(result.GetRuntime("mp_case_00"), result.m_avgRuntime);
    COUT(result.GetRuntime("mp_case_01"), 83176);
    COUT(result.GetRuntime("mp_case_67"), 96196);
    COUT(result.GetRuntime("mp_case_100"), 101100);
    COUT(result.GetRuntime("mp_case_101"), result.m_avgRuntime);
}

DEF_TOOL(pal_presort, "test presort and partition: --random")
{
    int nProcess = 4;
    int nCase = 100;

    mock::CTastMgr stTastMgr;
    FillSampleTast(stTastMgr, nCase, "mp");
    const tast::TastPool& tastPool = stTastMgr.GetTastPool();
    tast::TastList tastList = tast::MakeTastList(tastPool);

    // tast::CProcessWork work(tastList, nProcess, stTastMgr);
    tast::CProcessWork work(tastList, nProcess, *G_TASTMGR);
    work.m_runfile = "sample.run";
    work.Partition();
    COUT(work.m_tastList.size(), 100);

    tast::CRuntimeResult result;
    int nRead = result.ReadFile(work.m_runfile);
    COUT(nRead, 100);
    COUT(result.m_avgRuntime);

    int index = 0;
    for (auto& item : work.m_tastList)
    {
        DESC("%02d: %s: %ld", index++, item->m_name, result.GetRuntime(item->m_name));
    }

    std::vector<int64_t> sumRange;
    for (auto& range : work.m_ranges)
    {
        int64_t sum = 0;
        for (int i = range.first; i < range.second; ++i)
        {
            sum += result.GetRuntime(work.m_tastList[i]->m_name);
        }
        COUT(sum);
        sumRange.push_back(sum);
    }

    COUT(1.0 * sumRange.back() / sumRange.front(), 1.0, 0.01);
}


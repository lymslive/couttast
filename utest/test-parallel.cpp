#include "test-agent.h"
#include "parallel.h"
#include "classtast.hpp"
#include "coutstd.hpp"
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
    BIND_ARGV(nProcess, "process");
    BIND_ARGV(nCase, "case");
    BIND_ARGV(nError, "error");

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

int DiffRange(const std::vector<tast::IndexRange>& ranges)
{
    tast::IndexRange front = ranges[0];
    tast::IndexRange back = ranges.back();
    int frontDist = front.second - front.first;
    int backDist = back.second - back.first;
    return backDist - frontDist;
}

DEF_TAST(pal_slice, "test slice_index partition")
{
    DESC("partition: 100 / 4");
    {
        std::vector<tast::IndexRange> ranges = tast::slice_index(100, 4);
        COUT(ranges);
        COUT(DiffRange(ranges), 0);
    }

    DESC("partition: 99 / 4");
    {
        std::vector<tast::IndexRange> ranges = tast::slice_index(99, 4);
        COUT(ranges);
        COUT(DiffRange(ranges), -1);
    }

    DESC("partition: 101 / 4");
    {
        std::vector<tast::IndexRange> ranges = tast::slice_index(101, 4);
        COUT(ranges);
        COUT(DiffRange(ranges), -1);
    }
}

DEF_TAST(pal_runresult, "test deal with runtime result")
{
    const char* sample = "sample.run";
    tast::CTastRuntime result;
    int nRead = result.ReadFile(sample);
    COUT(nRead, 100);
    COUT(result.m_avgRuntime);

    COUT(result.GetRuntime("mp_case_00"), result.m_avgRuntime);
    COUT(result.GetRuntime("mp_case_01"), 83176);
    COUT(result.GetRuntime("mp_case_67"), 96196);
    COUT(result.GetRuntime("mp_case_100"), 101100);
    COUT(result.GetRuntime("mp_case_101"), result.m_avgRuntime);
}

DEF_TOOL(pal_presort, "test presort and partition: --prerun --random")
{
    int nProcess = 4;
    int nCase = 100;

    mock::CTastMgr stTastMgr;
    FillSampleTast(stTastMgr, nCase, "mp");
    const tast::TastPool& tastPool = stTastMgr.GetTastPool();
    tast::TastList tastList = tast::MakeTastList(tastPool);

    tast::CProcessWork work(tastList, nProcess, *G_TASTMGR);
    work.m_runfile = "sample.run";
    work.Partition();
    COUT(work.w_tastList.size(), 100);

    tast::CTastRuntime result;
    int nRead = result.ReadFile(work.m_runfile);
    COUT(nRead, 100);
    COUT(result.m_avgRuntime);

    int index = 0;
    for (auto& item : work.w_tastList)
    {
        DESC("%02d: %s: %ld", index++, item->m_name, result.GetRuntime(item->m_name));
    }

    std::vector<int64_t> sumRange;
    for (auto& range : work.m_range)
    {
        int64_t sum = 0;
        for (tast::TastEntry* it = range.first; it < range.last; ++it)
        {
            sum += result.GetRuntime((*it)->m_name);
        }
        COUT(sum);
        sumRange.push_back(sum);
    }

    if (!COUT(1.0 * sumRange.back() / sumRange.front(), 1.0, 0.01))
    {
        DESC("you may use --prerun and --cwd option to read sample.run file");
    }
}

struct PalPart
{
    int nProcess = 4;
    int nCase = 100;
    std::string runfile = "sample.run";
    tast::TastList tastList;
    std::vector<tast::TastList> section;
    tast::CTastRuntime presult;
    mock::CTastMgr stTastMgr;

    void setup()
    {
        FillSampleTast(stTastMgr, nCase, "mp");
        const tast::TastPool& tastPool = stTastMgr.GetTastPool();
        tastList = tast::MakeTastList(tastPool);

        COUT_ASSERT(presult.ReadFile(runfile), 100);
        COUT(presult.m_avgRuntime);

        section.resize(nProcess);
    }

    void teardown()
    {
        int index = 0;
        for (auto& item : tastList)
        {
            DESC("%02d: %s: %ld", index++, item->m_name, presult.GetRuntime(item->m_name));
        }

        int64_t sumTotal = 0;
        std::vector<int64_t> sumRange;
        for (auto& range : section)
        {
            int64_t sum = 0;
            for (auto& item : range)
            {
                sum += presult.GetRuntime(item->m_name);
                COUT(item->m_name);
            }
            COUT(sum);
            sumRange.push_back(sum);
            sumTotal += sum;
        }

        int64_t sumAverage = sumTotal / section.size();
        int64_t sumDiff = 0;
        for (auto sum : sumRange)
        {
            int64_t diff = sum - sumAverage;
            sumDiff = diff*diff;
        }

        COUT(sumAverage);
        COUT(sumDiff);

        if (!COUT(1.0 * sumRange.back() / sumRange.front(), 1.0, 0.01))
        {
            DESC("you may use --prerun and --cwd option to read sample.run file");
        }
    }
};

DEC_TOOL(PalPart, heapfill, "test min-heap and partition: --prerun --random")
{
    presult.MinHeapFill(tastList, section);
}

// more balance than heapfill although a bit slow
DEC_TOOL(PalPart, sortzigzag, "test sort ziazag and partition: --prerun --random")
{
    presult.SortZigzag(tastList, section);
}

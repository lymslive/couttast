#include "parallel.h"

#include <fstream>
#include <algorithm>

namespace tast
{

inline
int cpu_count()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

const int MAX_WORKER_COUNT = 32;

void fix_workers(int& workers)
{
    if (workers <= 0)
    {
        workers = cpu_count();
    }
    if (workers > MAX_WORKER_COUNT)
    {
        workers = MAX_WORKER_COUNT;
    }
}

typedef std::pair<int, int> IndexRange;
std::vector<IndexRange> slice_index(int total, int count)
{
    std::vector<IndexRange> ranges;
    int each = total / count;
    for (int i = 0; i < count; ++i)
    {
        int first = i * each;
        int last = (i+1) * each;
        if (i == count-1)
        {
            last = total;
        }
        ranges.push_back({first, last});
    }
    return ranges;
}

struct CRuntimeResult
{
    std::map<std::string, int64_t> m_mapRuntime;
    int64_t m_avgRuntime;

    int ReadFile(const std::string& file)
    {
        std::ifstream fin(file);
        if (!fin)
        {
            return 0;
        }
        int64_t sum = 0;
        int num = 0;
        std::string line;
        while (std::getline(fin, line))
        {
            CTastFoot foot;
            foot.ReadLine(line);
            if (foot.runtime > 0 && !foot.name.empty())
            {
                m_mapRuntime[foot.name] = foot.runtime;
                sum += foot.runtime;
                num++;
            }
        }
        m_avgRuntime = sum / num;
        return num;
    }

    int64_t GetRuntime(const std::string& name)
    {
        auto it = m_mapRuntime.find(name);
        if (it != m_mapRuntime.end())
        {
            return it->second;
        }
        return m_avgRuntime;
    }
};

struct CProcessWork
{
    const TastList& w_tastList; // input tast list to run
    CTastMgr& w_tastMgr;        // in which CTastMgr to run
    int m_workers = 0;          // dispatch how many workers to run
    TastList m_tastList;        // reordered tast list
    std::vector<IndexRange> m_ranges; // partition ranges of m_tastList
    CTastSummary m_summary;    // summary of workers
    std::string m_runfile;     // file to record runtime of each tast

    CProcessWork(const TastList& tastList, int workers, CTastMgr& tastMgr)
        : w_tastList(tastList), w_tastMgr(tastMgr), m_workers(workers)
    {
        fix_workers(m_workers);
        m_runfile = program_invocation_short_name;
        m_runfile += ".run";
    }

    // copy and sort
    void PreSort()
    {
        CRuntimeResult presult;
        if (presult.ReadFile(m_runfile) <= 0)
        {
            return;
        }

        m_tastList = w_tastList;
        auto comp = [&presult](const TastEntry& a, const TastEntry& b)
        {
            return presult.GetRuntime(a.first) <= presult.GetRuntime(b.first);
        };
        std::sort(m_tastList.begin(), m_tastList.end(), comp);

        TastList tmpList;
        tmpList.reserve(m_tastList.size());
        int each = m_tastList.size() / m_workers;
        for (int i = 0; i < m_workers; ++i)
        {
            for (int j = 0; j < each; ++j)
            {
                tmpList.push_back(m_tastList[i+m_workers*j]);
            }
        }

        // the last part may be a bit longer
        // should match slice_index() algorithm
        int last = each * m_workers;
        for (int i = last; i < m_tastList.size(); ++i)
        {
            tmpList.push_back(m_tastList[i]);
        }

        m_tastList.swap(tmpList);
    }

    void Partition()
    {
        PreSort();
        m_ranges = slice_index(w_tastList.size(), m_workers);
        // todo: --random, reorder each range before child process
    }

    void DealRange(const TastList& tastList, IndexRange range)
    {
        for (int i = range.first; i < range.second; ++i)
        {
            auto& item = tastList[i];
            w_tastMgr.RunTast(item.first, item.second);
        }
    }

    void ChildWork(int worker)
    {
        if (worker < 0 || worker >= m_workers) { return; }
        if (m_ranges.size() != m_workers) { return; }

        w_tastMgr.CoutDisable(COUT_MASK_ALL);
        w_tastMgr.CoutEnable(COUT_BIT_FOOT);
        w_tastMgr.SetPrint(nullptr);

        IndexRange range = m_ranges[worker];
        if (m_tastList.empty())
        {
            DealRange(w_tastList, range);
        }
        else 
        {
            DealRange(m_tastList, range);
        }
    }

    void ParentWork(FILE* fp)
    {
        if (fp == nullptr) { return; }

        std::ofstream fout(m_runfile);

        size_t cap = MAX_PRINT_BUFFER;
        char* ptr = (char*)::malloc(cap);
        int len = 0;
        while((len = ::getline(&ptr, &cap, fp)) > 0)
        {
            // len is the strlen(ptr) read, include '\n'
            // printf("%s", ptr);
            if (len > 0 && ptr[len-1] == '\n')
            {
                fout << ptr;
                ptr[len-1] = '\0';
                w_tastMgr.Print(COUT_BIT_FOOT, ptr);
            }

            CTastFoot foot;
            int nRead = foot.ReadLine(ptr, len);
            if (nRead > 0)
            {
                if (foot.failed)
                {
                    m_summary.fail++;
                    m_summary.failNames.push_back(foot.name);
                }
                else
                {
                    m_summary.pass++;
                }
            }
            else
            {
                DESC("parse bad foot: %s, Ret: %d", ptr, nRead);
            }
        }
        ::free(ptr);
    }

    int Summary()
    {
        std::string output;
        m_summary.Print(output);
        w_tastMgr.Print(COUT_BIT_LAST, output.c_str());
        return m_summary.fail;
        // todo: report random order if any fail
    }

    int ForkRun()
    {
        Partition();

        int fdPipe[2];
        if (::pipe(fdPipe) < 0)
        {
            perror("fail to create pipe for child process");
            return -1;
        }

        for (int i = 0; i < m_workers; ++i)
        {
            pid_t pid = ::fork();
            if (pid < 0)
            {
                ::perror("fail to fork child process");
                return -1;
            }
            else if (pid > 0) // current parent process
            {
                IndexRange range = m_ranges[i];
                DESC("forked child process[%d] for range[%d,%d)", pid, range.first, range.second);
            }
            else // forked child process
            {
                // close read end in parent, dup write end to STDOUT
                ::close(fdPipe[0]);
                if (fdPipe[1] != STDOUT_FILENO)
                {
                    if(::dup2(fdPipe[1], STDOUT_FILENO) != STDOUT_FILENO)
                    {
                        ::perror("fail to dup2 STDOUT");
                        return -1;
                    }
                    ::close(fdPipe[1]);
                }
                ChildWork(i);
                ::exit(0);
            }
        }

        // close write end in parent, read result from child process
        close(fdPipe[1]);
        FILE* fp = ::fdopen(fdPipe[0], "r");
        if (fp == nullptr)
        {
            ::perror("fail to open pipe[0] as file stream");
            return -1;
        }
        ParentWork(fp);
        ::fclose(fp);

        return Summary();
    }
};

int process_run(const TastList& tastList, int workers, CTastMgr* pTastMgr)
{
    if (pTastMgr == nullptr)
    {
        pTastMgr = G_TASTMGR;
    }

    CProcessWork work(tastList, workers, *pTastMgr);

    return work.ForkRun();
}

int process_run(const TastMap& tastMap, int workers, CTastMgr* pTastMgr)
{
    TastList tastList;
    for (auto& item : tastMap)
    {
        tastList.push_back(item);
    }
    return process_run(tastList, workers, pTastMgr);
}

} // end of namespace tast

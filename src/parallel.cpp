#include "parallel.h"
#include "filter.h"
#include "tastargv.hpp"
#include "coutdebug.hpp"

#include <unordered_map>
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
    int extra = total % count;

    int first = 0;
    for (int i = 0; i < count; ++i)
    {
        int last = first + each;
        if (i < extra)
        {
            last++;
        }
        if (i == count-1)
        {
            last = total;
        }
        ranges.push_back({first, last});
        first = last;
    }
    return ranges;
}

struct CTastRuntime
{
    std::unordered_map<std::string, int64_t> m_mapRuntime;
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

    void SortZigzag(TastList& tastList, std::vector<TastList>& section)
    {
        if (section.empty() || tastList.size() < section.size())
        {
            return;
        }

        auto comp = [this](const TastEntry& a, const TastEntry& b)
        {
            // fixme: construct std::string each time
            return this->GetRuntime(a->m_name) < this->GetRuntime(b->m_name);
        };
        std::sort(tastList.begin(), tastList.end(), comp);

        int workers = section.size();
        int work = 0;
        int step = 1;
        for (auto item : tastList)
        {
            section[work].push_back(item);
            if (step == 1 && work == workers - 1)
            {
                step = -1;
            }
            else if (step == -1 && work == 0)
            {
                step = 1;
            }
            else
            {
                work = work + step;
            }
        }
    }

    void MinHeapFill(TastList& tastList, std::vector<TastList>& section)
    {
        if (section.empty() || tastList.size() < section.size())
        {
            return;
        }

        std::vector<int64_t> sumTime(section.size(), 0);

        std::vector<int> index;
        for (int i = 0; i < section.size(); ++i)
        {
            index.push_back(i);
        }

        auto comp = [&sumTime](int a, int b)
        {
            return sumTime[a] > sumTime[b];
        };

        auto it = tastList.begin();
        auto itEnd = tastList.end();

        // fill each section one item before make min heap
        for (int i = 0; i < section.size(); ++i)
        {
            section[i].push_back(*it);
            sumTime[i] +=GetRuntime((*it)->m_name);
            ++it;
        }

        // fill the remain items.
        while (it != itEnd)
        {
            std::make_heap(index.begin(), index.end(), comp);
            int work = index[0];
            section[work].push_back(*it);
            sumTime[work] +=GetRuntime((*it)->m_name);
            ++it;
        }
    }
};

struct TastRange
{
    TastEntry* first = nullptr;
    TastEntry* last = nullptr;

    int count() const
    {
        return last - first;
    }
};

struct CProcessWork
{
    TastList& w_tastList; //< input tast list to run
    CTastMgr& w_tastMgr;        //< in which CTastMgr to run
    int m_workers = 0;          //< dispatch how many workers to run
    std::vector<TastRange> m_range; //< range for each worker, point into
                                    //< w_tastList or m_section item.
    std::vector<TastList> m_section; //< split tast list for each worker.
    CTastSummary m_summary;    //< summary of workers
    std::string m_runfile;     //< file to record runtime of each tast

    CProcessWork(TastList& tastList, int workers, CTastMgr& tastMgr)
        : w_tastList(tastList), w_tastMgr(tastMgr), m_workers(workers)
    {
        fix_workers(m_workers);
        CheckPrerun();
    }

    bool HasRandom() const
    {
        return CTastArgv(&w_tastMgr).HasKey("random");
    }

    bool CheckPrerun()
    {
        if (false == CTastArgv(&w_tastMgr).BindValue(m_runfile, "prerun"))
        {
            return false;
        }
        if (m_runfile.empty() || m_runfile == "--")
        {
            m_runfile = program_invocation_short_name;
            m_runfile += ".run";
        }
        return true;
    }

    // sort and re arrange
    bool PreSort()
    {
        CTastRuntime presult;
        if (presult.ReadFile(m_runfile) <= 0)
        {
            return false;
        }

        // zigzag fill the sorted test in section for each worker.
        m_section.resize(m_workers);
        presult.SortZigzag(w_tastList, m_section);

        m_range.resize(m_workers);
        for (int work = 0; work < m_workers; ++work)
        {
            m_range[work].first = &(m_section[work].front());
            m_range[work].last = &(m_section[work].back()) + 1;
        }

        if (HasRandom())
        {
            RandomRange();
        }

        return true;
    }

    void Partition()
    {
        if (!m_runfile.empty() && PreSort())
        {
            return;
        }

        std::vector<IndexRange> slice = slice_index(w_tastList.size(), m_workers);
        for (auto& item : slice)
        {
            TastRange range;
            range.first = &w_tastList[item.first];
            range.last = &w_tastList[item.second];
            m_range.push_back(range);
        }
    }

    void RandomRange()
    {
        for (auto& range : m_range)
        {
            random_tast(range.first, range.last);
        }
    }

    void DealRange(const TastRange& range)
    {
        for (TastEntry* it = range.first; it < range.last; ++it)
        {
            w_tastMgr.RunTast(**it);
        }
    }

    void ChildWork(int worker)
    {
        if (worker < 0 || worker >= m_workers) { return; }
        if (m_range.size() != m_workers) { return; }

        w_tastMgr.CoutDisable(COUT_MASK_ALL);
        w_tastMgr.CoutEnable(COUT_BIT_FOOT);
        w_tastMgr.SetPrint(nullptr);

        DealRange(m_range[worker]);
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
                if (fout)
                {
                    fout << ptr;
                }
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

        // report run order if any fail
        if (m_summary.fail != 0 || w_tastMgr.CoutMask(COUT_BIT_DESC))
        {
            output.clear();
            RerportRange(output, w_tastList);
            w_tastMgr.Print(output.c_str());
        }

        // report incorrect amount of run cases.
        int total = m_summary.fail + m_summary.pass;
        if (total != w_tastList.size())
        {
            output.clear();
            output += "!! Total run cases: ";
            output += std::to_string(total);
            output += ", while expect to run cases: ";
            output += std::to_string(w_tastList.size());
            w_tastMgr.Print(COUT_BIT_LAST, output.c_str());
        }

        return m_summary.fail;
    }

    void RerportRange(std::string& output, const TastList& tastList)
    {
        int i = 0;
        for (auto& range : m_range)
        {
            std::string order("-- process");
            order.append("[").append(std::to_string(i)).append("]:");
            i++;

            for (TastEntry* it = range.first; it < range.last; ++it)
            {
                order.append(" ").append((*it)->m_name);
            }

            if (!output.empty())
            {
                output.append("\n");
            }
            output.append(order);
        }
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
                COUT_DBG("forked child process[%d] to test %d cases", pid, m_range[i].count());
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

/* ---------------------------------------------------------------------- */

int process_run(TastList& tastList, int workers, CTastMgr* pTastMgr)
{
    if (pTastMgr == nullptr)
    {
        pTastMgr = G_TASTMGR;
    }

    CProcessWork work(tastList, workers, *pTastMgr);

    return work.ForkRun();
}

int process_run(const TastPool& tastPool, int workers, CTastMgr* pTastMgr)
{
    TastList tastList = MakeTastList(tastPool);
    return process_run(tastList, workers, pTastMgr);
}

} // end of namespace tast

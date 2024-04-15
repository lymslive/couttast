#include "test-agent.h"
#include "agent.h"
#include "extra-macros.hpp"

#include <unistd.h>

struct CTastSample : public tast::CTastCase
{
    CTastSample(tast::CTastMgr* pTastMgr, const std::string& desc, const std::string& file, int line, bool autoRun = true)
    {
        ctor(desc, file, line, autoRun);
    }

    virtual void run() override
    {
        int ms = 80 + rand() % 40;
        usleep(ms * 1000);

        if (randError > 0)
        {
            if (rand() % 100 < randError)
            {
                COUT("simulate error", "actually no error");
            }
        }
    }

    static int randError;
};

int CTastSample::randError = 0;

void RandError(int percent)
{
    CTastSample::randError = percent;
}

void FillSampleTast(tast::CTastMgr& stTastMgr, int nCount, const char* prefix)
{
    if (prefix == nullptr || prefix[0] == '\0')
    {
        prefix = "sample";
    }

    char buffer[64] = {0};
    for (int i = 0; i < nCount; ++i)
    {
        snprintf(buffer, sizeof(buffer), "%s_case_%02d", prefix, i+1);
        std::string name(buffer);

        snprintf(buffer, sizeof(buffer), "filled %s test: %d", prefix, i+1);
        std::string desc(buffer);

        snprintf(buffer, sizeof(buffer), "test-%s-%d.cpp", prefix, i/10+1);
        std::string file(buffer);

        int line = 10 * (i%10) + 1;
        bool autoRun = ((i+1)%10 != 0);

        tast::CTastCase* pTastCase = new CTastSample(&stTastMgr, desc, file, line, autoRun);
        stTastMgr.AddTast(name, pTastCase);
    }
}

void RandSleep()
{
    srand(time(nullptr));
    int ms = rand() % 50 + 100;
    // printf("%d\n", ms);
    // DESC("%d", ms);
    COUT(ms);
    usleep(ms * 1000);
}

DEF_TOOL(avg_fun_long, "test average time of function, may long time")
{
    int64_t us = tast::average_time(RandSleep);
    COUT(us);

    us = tast::average_time(RandSleep, 100, 0);
    COUT(us);
    COUT(1.0 * us / (125 * 1000), 1.0, 0.1);
}

DEF_TOOL(arg_cwd, "test --cwd to change current working directory")
{
    char* cwd = ::get_current_dir_name();
    COUT(cwd);
    ::free(cwd);
}

#include "test-agent.h"
#include "agent.h"
#include "extra-macros.hpp"

#include <unistd.h>

struct CTastSample
{
    static int randError;
    static void run()
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
};

int CTastSample::randError = 0;

void RandError(int percent)
{
    CTastSample::randError = percent;
}

void FillSampleTast(mock::CTastMgr& stTastMgr, int nCount, const char* prefix)
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

        const char* pszName = stTastMgr.SaveName(name);
        const char* pszDesc = stTastMgr.SaveName(desc);
        const char* pszFile = stTastMgr.SaveName(file);
        // tast::CTastCase* pTastCase = new CTastSample(pszName, pszDesc, pszFile, line, autoRun);
        // stTastMgr.AddTast(pTastCase);
        tast::CTastCase tast(CTastSample::run, pszName, pszDesc, pszFile, line, autoRun);
        stTastMgr.AddTast(tast);
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
    DESC("use arverage_time() in default argument");
    int64_t us = tast::average_time(RandSleep);
    COUT(us);

    DESC("use arverage_time() with optional argument (100,0)");
    us = tast::average_time(RandSleep, 100, 0);
    COUT(us);
    COUT(1.0 * us / (125 * 1000), 1.0, 0.1);

    DESC("use TIME_TAST marco instead");
    us = TIME_TAST(RandSleep, 100, 0);
    COUT(us);
    COUT(1.0 * us / (125 * 1000), 1.0, 0.1);
}

DEF_TOOL(arg_cwd, "test --cwd to change current working directory")
{
    char* cwd = ::get_current_dir_name();
    COUT(cwd);
    ::free(cwd);
}

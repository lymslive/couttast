#include "format.h"

// defined in test-tinyini.cpp
std::ostream& operator<<(std::ostream& os, const tast::CTinyCli& cli);

// Parse a string as cli argument simply seprated by space, may modify it in place.
void ParseCli(tast::CTinyCli& cli, std::string& line)
{
    std::vector<char*> argv;
    argv.push_back(nullptr); // ignore argv[0] as program name,
                             // push "" would have warning.
    char* ptr = const_cast<char*>(line.c_str());
    while (*ptr != '\0')
    {
        while (*ptr == ' ' || *ptr == '\t') ++ptr;
        if (*ptr == '\0') break;
        char* arg = ptr;
        while (!(*ptr == ' ' || *ptr == '\t' || *ptr == '\0')) ++ptr;
        argv.push_back(arg);
        if (*ptr != '\0') *ptr++ = '\0';
    }
    cli.ParseCli((int)argv.size(), &argv[0]);
}

DEF_TAST(format_config, "parse cli into config struct")
{
    {
        std::string line = "--help";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.help, 'h');
    }

    {
        std::string line = "-h";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.help, 'h');
    }

    {
        std::string line = "--list";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.list, 'l');
    }

    {
        std::string line = "-l";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.list, 'l');
    }

    {
        std::string line = "--List";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.list, 'L');
    }

    {
        std::string line = "-L";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.list, 'L');
        COUT((int)cfg.job, -1);
    }

    {
        std::string line = "--cout=any --job=4";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.cout, "any");
        COUT((int)cfg.job, 4);
    }

    {
        std::string line = "--cout --job";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.cout, "--");
        COUT((int)cfg.job, 0);
    }

    {
        std::string line = "--cout= --job=";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(line); // line is modified, replace ' ' to '\0'
        COUT(cli);
        COUT(cfg.cout.empty(), true);
        COUT((int)cfg.job, 0);
    }

    {
        std::string line = "--all --notool + +foo -b -bar";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.all, true);
        COUT(cfg.notool, false);
        COUT(cfg.adds.size(), 2);
        COUT(cfg.subs.size(), 1);
        COUT(cfg.subs[0], "bar");
    }

    {
        std::string line = "all --notool + +foo -b -bar";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.all, false);
        COUT(cfg.notool, true);
        COUT(cfg.adds.size(), 3);
        COUT(cfg.adds[0], "all");
        COUT(cfg.subs.size(), 1);
        COUT(cfg.subs[0], "bar");
    }

    {
        std::string line = "-all --notool + +foo -b -bar";
        DESC("input line: %s", line.c_str());
        tast::CTinyCli cli;
        ParseCli(cli, line);
        tast::CTastConfig cfg;
        cfg.ParseCli(cli);
        COUT(cli);
        COUT(cfg.all, false);
        COUT(cfg.notool, true);
        COUT(cfg.adds.size(), 2);
        COUT(cfg.adds[0], "+");
        COUT(cfg.adds[1], "foo");
        COUT(cfg.subs.size(), 2);
        COUT(cfg.subs[0], "all");
    }

}

DEF_TAST(format_foot, "test parse foot message")
{
    {
        const char* str = "<< [PASS] 0 filter_basic 1357 us";
        DESC("foot line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 4);
        COUT(foot.name, "filter_basic");
        COUT(foot.runtime, 1357);
        COUT(foot.failed, false);
        COUT(foot.count, 0);
    }

    {
        const char* str = "<< [FAIL] 0 filter_basic 1357 us";
        DESC("may bad line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 4);
        COUT(foot.name, "filter_basic");
        COUT(foot.runtime, 1357);
        COUT(foot.failed, true);
        COUT(foot.count, 0);
    }

    {
        const char* str = "<< [PASS] 211 filter_basic 1357 us";
        DESC("bad line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 2);
        COUT(foot.name.empty(), true);
        COUT(foot.runtime, 0);
        COUT(foot.failed, false);
        COUT(foot.count, 211);
    }

    {
        const char* str = "<< [PASS] -211 filter_basic 1357 us";
        DESC("bad line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, -1);
        COUT(foot.name.empty(), true);
        COUT(foot.runtime, 0);
        COUT(foot.failed, false);
        COUT(foot.count, 0);
    }

    {
        const char* str = "<< [FAIL] 1 except_macr 762 us";
        DESC("foot line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 4);
        COUT(foot.name, "except_macr");
        COUT(foot.runtime, 762);
        COUT(foot.failed, true);
        COUT(foot.count, 1);
    }

    {
        const char* str = "<< [FAIL] 3 except_macr 762 us";
        DESC("foot line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 4);
        COUT(foot.name, "except_macr");
        COUT(foot.runtime, 762);
        COUT(foot.failed, true);
        COUT(foot.count, 3);
    }

    {
        const char* str = "<< [fail] 3 except_macr 762 us";
        DESC("bad line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 0);
        COUT(foot.name.empty(), true);
        COUT(foot.runtime, 0);
        COUT(foot.failed, false);
    }

    {
        const char* str = "<< [PASS] 0 filter_basic 1357";
        DESC("bad line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, -4);
        COUT(foot.name, "filter_basic");
        COUT(foot.runtime, 1357);
        COUT(foot.failed, false);
        COUT(foot.count, 0);
    }

    {
        const char* str = "<< [PASS] 0 filter_basic 13.57 us";
        DESC("bad line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, -3);
        COUT(foot.name, "filter_basic");
        COUT(foot.runtime, 0);
        COUT(foot.failed, false);
        COUT(foot.count, 0);
    }

    {
        const char* str = "<< [FAIL]1 filter_basic 13.57 us";
        DESC("bad line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 0);
        COUT(foot.name.empty(), true);
        COUT(foot.runtime, 0);
        COUT(foot.failed, false);
        COUT(foot.count, 0);
    }

    {
        const char* str = "<< [FAIL] 12";
        DESC("summary line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 2);
        COUT(foot.name.empty(), true);
        COUT(foot.runtime, 0);
        COUT(foot.failed, true);
        COUT(foot.count, 12);
    }

    {
        const char* str = "<< [PASS] 112";
        DESC("summary line: %s", str);
        tast::CTastFoot foot;
        int nRead = foot.ReadLine(str, strlen(str));
        COUT(nRead, 2);
        COUT(foot.name.empty(), true);
        COUT(foot.runtime, 0);
        COUT(foot.failed, false);
        COUT(foot.count, 112);
    }
}

#include "couttast.h"
#include "tinyini.h"

DEF_TAST(tinyini_line, "test parse from lines")
{
    tast::CTinyIni ini;
    ini.ParseLines({
            "xyz=123",
            "abc=val",
            "--flag",
            "-vv",
            "hello",
            "world",
            "--section.xyz=456",
            "section.abc=foo"
            });

    int xyz = 0;
    bool has_xyz = ini.GetValue(xyz, "xyz");
    COUT(has_xyz, true);
    COUT(xyz, 123);

    std::string abc;
    bool has_abc = ini.GetValue(abc, "abc");
    COUT(has_abc, true);
    COUT(abc, "val");

    {
        int xyz = 0;
        bool has_xyz = ini.GetValue(xyz, "section", "xyz");
        COUT(has_xyz, true);
        COUT(xyz, 456);

        std::string abc;
        bool has_abc = ini.GetValue(abc, "section", "abc");
        COUT(has_abc, true);
        COUT(abc, "foo");
    }

    {
        int xyz = 0;
        bool has_xyz = ini.GetValue(xyz, "section.xyz");
        COUT(has_xyz, true);
        COUT(xyz, 456);

        std::string abc;
        bool has_abc = ini.GetValue(abc, "section.abc");
        COUT(has_abc, true);
        COUT(abc, "foo");
    }

    COUT(ini);
}

DEF_TAST(tinyini_file, "test read from file")
{
    const char* file = "sample.ini";
    tast::CTinyIni ini;

    bool bLoadFile = ini.LoadIni(file);
    COUT_ASSERT(bLoadFile, true);
    COUT(ini);

    int xyz = 0;
    ini.GetValue(xyz, "xyz");
    COUT(xyz, 123);
    ini.GetValue(xyz, "section.xyz");
    COUT(xyz, 456);
    ini.GetValue(xyz, "another.xyz");
    COUT(xyz, 789);

    auto& argv = ini.m_vecArg;
    COUT(argv.size(), 3);
    COUT(argv[0], "hello");
    COUT(argv.back(), "world");
}

DEF_TAST(tinycli_option, "test access cli option with macro")
{
    int nCountBefore = TAST_OPTION.size();
    COUT(nCountBefore);
    COUT(TAST_OPTION["xyz"], "123");
    COUT(TAST_OPTION["section.xyz"], "456");
    COUT(TAST_OPTION["section.abc"], "foo");
    COUT(TAST_OPTION["none-key"], "");
    int nCountAfter = TAST_OPTION.size();
    COUT(nCountAfter, nCountBefore + 1);
}

// argument should include `global` to run this case, and then view cli.
DEF_TOOL(tinycli_global, "test global cli merged with ini")
{
    int xyz = 0;
    tast::GetOption("xyz", xyz);
    COUT(xyz, 123);
    tast::GetOption("section.xyz", xyz);
    COUT(xyz, 456);
    tast::GetOption("another.xyz", xyz);
    COUT(xyz, 789);

    auto& argv = tast::GetArguments();
    COUT(argv);

    COUT(*tast::CTastMgr::GetInstance());

    DESC("sleep 1 second, view the run time");
    sleep(1);
}


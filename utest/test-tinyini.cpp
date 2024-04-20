#include "couttast.h"
#include "tinyini.h"
#include "tastargv.hpp"
#include "coutstd.hpp"

/// operator<< overload for tast::CTinyCli in global namesapce.
std::ostream& operator<<(std::ostream& os, const tast::CTinyCli& cli);

std::ostream& operator<<(std::ostream& os, const tast::CTinyCli& cli)
{
    os << "...\n";
    os << "option: " << cli.m_mapOption << "\n";
    os << "argument: " << cli.m_vecArg;
    return os;
}

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

    tast::CTastArgv argv(&ini);

    COUT(argv["xyz"], "123");
    COUT(argv["xzy"].empty(), true);
    COUT(argv["flag"], "--");
    COUT(argv["vv"], "-");

    int xyz = 0;
    bool has_xyz = argv.BindValue(xyz, "xyz");
    COUT(has_xyz, true);
    COUT(xyz, 123);

    std::string abc;
    bool has_abc = argv.BindValue(abc, "abc");
    COUT(has_abc, true);
    COUT(abc, "val");

    {
        int xyz = 0;
        bool has_xyz = argv.BindValue(xyz, "section.xyz");
        COUT(has_xyz, true);
        COUT(xyz, 456);

        std::string abc;
        bool has_abc = argv.BindValue(abc, "section.abc");
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

    {
        tast::CTastArgv argv(&ini);

        int xyz = 0;
        argv.BindValue(xyz, "xyz");
        COUT(xyz, 123);
        argv.BindValue(xyz, "section.xyz");
        COUT(xyz, 456);
        argv.BindValue(xyz, "another.xyz");
        COUT(xyz, 789);

        COUT(argv[0], "hello");
    }

    {
        auto& argv = ini.m_vecArg;
        COUT(argv.size(), 3);
        COUT(argv[0], "hello");
        COUT(argv.back(), "world");
    }
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

DEF_TAST(tinycli_global, "test global cli merged with ini")
{
    int xyz = 0;
    BIND_ARGV(xyz);
    COUT(xyz, 123);
    BIND_ARGV(xyz, "section.xyz");
    COUT(xyz, 456);
    BIND_ARGV(xyz, "another.xyz");
    COUT(xyz, 789);

    auto& argv = TAST_ARGV.Argument();
    COUT(argv);

    COUT(*tast::CTastMgr::GetInstance());
}


#include "systemtast.h"
#include "extra-macros.hpp"

DEF_TAST(system_ls, "test simple command ls")
{
    DESC("execute ls command in uttest/ directory");
    COUT_SYSTEM("ls");
    COUT_SYSTEM("ls test-*");
    COUT_SYSTEM("ls nofile.null", 2);
    COUT_SYSTEM("ls test-systemtast.cpp", 0);

    DESC("default match output by ignore space in both end");
    COUT_SYSTEM("ls test-systemtast.cpp | wc -l", 0, "1");
    COUT_SYSTEM("ls test-systemtast.cpp | wc -l", 0, "1\n");
    COUT_SYSTEM("ls test-systemtast.cpp | wc -l", 0, "1 ");
    COUT_SYSTEM("ls test-systemtast.cpp | wc -l", " 1");
    COUT_SYSTEM("ls test-systemtast.cpp | wc -l", " 1 ");

    DESC("match output exactly equal, most command output a newline in the end");
    COUT_SYSTEM("ls test-systemtast.cpp | wc -l", 0, "1\n", tast::STRING_MATCH_EQUAL);
    COUT_SYSTEM("ls test-systemtast.cpp | wc -l", 0, "1", tast::STRING_MATCH_EQUAL);
    COUT_ERROR(1);
}

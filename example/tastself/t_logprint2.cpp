#include "tinytast.hpp"
#include "logprint.hpp"

// also test link .hpp in multi objects

DEF_TAST(strbit, "tast utd::strbit")
{
    COUT(utd::strbit(0), "00000000 00000000 00000000 00000000");
    COUT(utd::strbit(1), "00000000 00000000 00000000 00000001");
    COUT(utd::strbit(128), "00000000 00000000 00000000 10000000");
    COUT(utd::strbit(1024), "00000000 00000000 00000100 00000000");
    COUT(utd::strbit(0xFF), "00000000 00000000 00000000 11111111");
    COUT(utd::strbit((short)0xFFFF), "11111111 11111111");
    COUT(utd::strbit(0x09ABCDEF));
    COUT(utd::strbit(0x12345678));
}

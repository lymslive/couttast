/// A tiny trival `main` for `tinytast` or `couttast` framework.
/// May include it into one of and at least one of test case cpp files,
/// then compile and link it to other test case objects.
/// Alternative way is link to libtinytast.a or libcouttast, where also include this main.
/// Note: Can use macro `USE_COUTTAST_FULLIB` to switch to extend `couttast.h`
/// instead of basic `tinytast.hpp`, but need also link to `libcouttast`
/// ofcourse.
#ifdef USE_COUTTAST_FULLIB

#include "couttast.h"
int main(int argc, char* argv[])
{
    return tast::main(argc, argv);
}

#else

#include "tinytast.hpp"
int main(int argc, char* argv[])
{
    return RUN_TAST(argc, argv);
}
#endif

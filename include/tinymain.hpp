/** 
 * @file tinymain.hpp
 * @author lymslive
 * @date 2024-05-08
 * @brief A tiny trival `main` for `tinytast` or `couttast` framework.
 * @details 
 * May include it into one of and at most one of test case cpp files,
 * then compile and link it to other test case objects.
 * Alternative way is link to `libtinytast.a` or `libcouttast.a`, where also include this main.
 * @note Can use macro `USE_COUTTAST_FULLIB` to switch to extend `couttast.h`
 * instead of basic `tinytast.hpp`, but need also link to `libcouttast.a`
 * ofcourse.
 * */
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

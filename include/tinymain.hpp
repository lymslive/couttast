/// A tiny trival `main` for `tinytast` framework.
/// May include it into one of and at least one of test case cpp files,
/// then compile and link it to other test case objects.
/// Alternative way is link to libtinytast.a, where just include this main.
#include "tinytast.hpp"

int main(int argc, char* argv[])
{
    return RUN_TAST(argc, argv);
}

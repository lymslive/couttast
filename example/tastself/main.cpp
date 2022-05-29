#include "tinytast.hpp"

int main(int argc, char* argv[])
{
    printf("override main() to run tastself:\n");
    return RUN_TAST(argc, argv);
}

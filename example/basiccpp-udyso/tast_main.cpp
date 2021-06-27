#include "tinytast.hpp"

// 前向声明 C 接口，可放单独头文件
extern "C"
{
int tast_main(int argc, char* argv[]);
}

void local_init()
{
    DESC("local_init() work");
}

int tast_main(int argc, char* argv[])
{
    local_init();
    return RUN_TAST(argc, argv);
}

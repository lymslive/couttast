#include "tinytast.hpp"
#include "string.h"

void temp_tast()
{
    int i;
    DESC("未初始化整数");
    COUT(i);

    // CODE(i = 0);
    i = 0;
    COUT(i, 0);

    COUT(++i, i++);
    COUT(i);

    int* pi = NULL;
    DESC("COUT_ASSERT 必须用双参数断言，失败返回");
    // COUT_ASSERT(pi);
    COUT_ASSERT(pi != NULL, true);
    DESC("不该运行至此");
}

int main(int argc, char* argv[])
{
    // 自定义 main ，可以先处理自己的命令行参数
    if (argc > 1 && strcmp(argv[1], "--tmp") == 0)
    {
        DESC("测试普通 void 函数");
        // TAST(temp_tast);
        temp_tast();
        return RUN_TAST(argc-1, argv+1);
    }
	return RUN_TAST(argc, argv);
}

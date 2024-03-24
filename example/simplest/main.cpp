#include "tinytast.hpp"

int main(int argc, char** argv)
{
    return RUN_TAST(argc, argv);
}

DEF_TAST(test_name, "测试用例说明")
{
    COUT(sizeof(int));
    COUT(sizeof(int), 4);
    COUT(sizeof(int)==4, true);
    DESC("string 大小可能依编译器版本实现而不同");
    COUT(sizeof(std::string), 32);
}


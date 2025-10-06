#include "tinytast.hpp"
#include "extra-macros.hpp"
#include "gtest-macros.hpp"
#include <exception>

DEF_TOOL(tinytast_cout, "test cout output control, contains failed statement")
{
    DESC("make an addtion");
    int add = 1 + 2;
    COUT(add);
    COUT(add, 3);

    G_TASTMGR->CoutDisable(tast::COUT_BIT_PASS);
    COUT(add * 3, 9);
    G_TASTMGR->CoutDisable(tast::COUT_BIT_DESC);
    DESC("you should not see this line as diabled");
    // even failed statement can diable output
    G_TASTMGR->CoutDisable(tast::COUT_BIT_FAIL);
    COUT(add, 4); 

    G_TASTMGR->CoutEnable(tast::COUT_MASK_ALL);
    DESC("you should see this line again");

    DESC("clear error to let this case pass");
    COUT_ERROR(1);
}

DEF_TOOL(tinytast_sizeof, "view sizeof class in tast namespace")
{
    using namespace tast;
    COUT(sizeof(CTinyCli));
    COUT(sizeof(CTastCase));
    COUT(sizeof(CTastMgr));
    COUT(sizeof(CTastBuilder));
    COUT(sizeof(CStatement));
}

DEF_TAST(tinytast_compare, "basic cout compare assert")
{
    DESC("view sizeof int and ptr");
    COUT(sizeof(int));
    COUT(sizeof(int*));

    COUT(1+1, 2);
    COUT(1+1 == 2, true);
    COUT(1+1 == 3, false);

    COUT(1.0 + 1.0, 2.0);
}

DEF_TAST(tinytast_float, "compare float number")
{
    double a = 1.0 / 3.0;
    double b = 10.0 / 30.0;
    COUT(a);
    COUT(b);

    if (a == b)
    {
        DESC("double a == b passed if branch");
    }
    else
    {
        DESC("double a == b NOT passed if branch");
    }

    COUT(a, b);
    COUT(a == b);
    COUT(a, b, 0.001);
    COUT(a, b, 0.00001);

    a = 0.314;
    b = 0.314;
    COUT(a, b);
    COUT(a == b);
    COUT(a, b, 0.001);
    COUT(a, b, 0.00001);
    COUT(a, 0.3142, 0.00001);
    COUT_ERROR(1);
}

DEF_TOOL(tinytast_tictoc, "test tic toc")
{
    usleep(100 * 1000); // pre action
    TIME_TIC;
    for (int i = 0; i < 1; ++i)
    {
        usleep(200 * 1000); // call some complex function
    }
    TIME_TOC;
    usleep(100 * 1000); // post action
}

DEF_TOOL(tinytast_notictoc, "test time without tic toc")
{
    usleep(100 * 1000); // pre action
    for (int i = 0; i < 1; ++i)
    {
        usleep(200 * 1000); // call some complex function
    }
    usleep(100 * 1000); // post action
}

DEF_TOOL(tinytast_avgtime, "test average time")
{
    int64_t avg = TIME_TAST("tinytast_tictoc", 5);
    COUT(1.0*avg / (200*1000), 1.0, 0.1);
    avg = TIME_TAST("tinytast_notictoc", 5, 0);
    COUT(1.0*avg / (400*1000), 1.0, 0.1);
}

DEF_TAST(gtest_simacro, "simulate macro in gtest style")
{
    EXPECT_EQ(1+1, 2);
    ASSERT_EQ(1+1, 2);
    EXPECT_NE(1+1, 3);
    EXPECT_LT(1+1, 3);
    EXPECT_LE(1+1, 3);

    EXPECT_STREQ("hello", "hello");
    EXPECT_STRNE("hello", "hella");
}

DEF_TAST(duplicate, "may duplicate test name in global")
{
    COUT(1+1, 2);
}

namespace nest
{

DEF_TAST(duplicate, "may duplicate test name in namespace")
{
    COUT(2+2, 4);
}

namespace tool
{

DEF_TAST(duplicate, "may duplicate test name in namespace")
{
    COUT(3+3, 6);
}

} // endof nest::

}

DEF_TAST(string_compare, "Test COUT and COUT_PTR difference for const char*")
{
    const char* str1 = "hello";
    const char* str2 = "hello"; 
    const char* str3 = "world";
    char* str4 = new char[6]; // 动态分配，确保不同指针
    ::strcpy(str4, "hello");
    
    DESC("Test COUT behavior - compares string content using strcmp");
    COUT(str1, str2);      // 相同内容，相同指针 - 应该通过
    COUT(::strcmp(str1, str4) == 0, true);  // 相同内容，不同指针 - 验证内容相同
    COUT(::strcmp(str1, str3) == 0, false); // 不同内容 - 验证内容不同
    
    DESC("Test COUT_PTR behavior - compares pointer addresses");
    COUT_PTR(str1, str1);  // 相同指针 - 应该通过
    COUT_PTR(str1 == str2, true);  // 相同指针（字面量优化） - 验证指针相同
    COUT_PTR(str1 == (const char*)str4, false); // 不同指针 - 验证指针不同
    
    DESC("Test with string literals");
    COUT("hello", "hello");        // 相同内容 - 应该通过（比较内容）
    COUT_PTR("hello" == "hello", true); // 相同指针（字面量优化） - 验证指针相同
    
    DESC("Test with null pointers");
    const char* null1 = NULL;
    const char* null2 = NULL;
    COUT(null1 == null2, true);    // 都为空 - 验证都为空
    COUT(null1 == str1, false);     // 一个为空，一个不为空 - 验证不等
    COUT_PTR(null1, null2); // 都为空 - 应该通过
    COUT_PTR(null1 == str1, false);  // 一个为空，一个不为空 - 验证指针不等
    
    // 清理动态分配的内存
    delete[] str4;
}

#ifdef USE_TINY_MAIN
#include "tinymain.hpp"
#endif

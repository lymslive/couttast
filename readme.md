# tast: 体验测试 v0.1

不叫 **test** 而叫 **tast** ，是因为与常规的“测试”专业术语有点不一样，也比较
轻量。

## 背景概述

考虑最原始的手动测试，一般可用 `cout` 大法向终端输出一些信息，在开发过程中通过
观测分析输出判断开发的功能是否合意。然后可以将输出信息用重定向持久保存至文件，
比如命名为 `*.cout` 。可供后面作回归测试，如修改了被测代码，可将输出定向至另外的
文件，与原输出文件用 diff 比较。

而这里的 tast 库提人提供 `COUT` 宏除了简单打印表达式的值，也可以加上额外的预期
值，如此可以比较判断是否符合预期值（类似 gtest 的 EXPECT 宏组），不必保存输出
文件用 diff 也可判断测试是否通过。

## 宏定义详解

### COUT 语句宏

顾名思义，`COUT` 就是将一个表达的值打印出来。支持任意类型，只要其支持 `<<` 操
作符。

* 单参数 `COUT(expr)` ：打印 `expr` 表达式字面量及其值，使其输出信息更有可读意义，且更易找到源文件互为参照。
* 双参数 `COUT(expr, expect)` ：在单参数的基础上扩展，比较 `expr` 与 `expect`
  的值，如果相等，在行尾打上 '[OK]' 的标记，否则打上 `[NO]` 的标记。表达式与预
  期值的类型可以不同，但要求支持 `==` 操作重载。

但是应该避免浮点数比较，因为 `==` 有误差不靠谱，果真需要，可显式如下调用：

```cpp
COUT(abs(expr-expect) <= limit, true);
```

### DESC 描叙宏

另外提供两个纯输出函数，仅为改善输出信息的可读性，可选使用。

* `DESC(msg, ...)`：类似 `printf` 调用。
* `CODE(statement)`：执行语句前打印出该语句。

### TAST 宏半自动测试

* `TAST(test_fun)`：调用测试函数名（`void()`类型的函数指针）
* `TAST_RESULT`：打印测试统计信息

## 使用示例

```cpp
#include "tast_util.hpp"

void ordinary_fun() {}

void test_fun1() {} // 使用 COUT 查看或测试语句结果
void test_fun2() {}

int main(int argc, char* argv[])
{
    TAST(test_fun1);
    ordinary_fun();
    TAST(test_fun2);

    return TAST_RESULT;
}
```

不失灵活性，完全可以按常规编程习惯定义一些函数，只要这些函数用到 `COUT` 宏就
能输出与比较。然后手动在 `main()` 中组织调用这些函数。不过若是 `void()` 类型的
函数，则可用 `TAST()` 宏来调用，将会额外跟踪加入统计信息。

统计信息可最后由 `TAST_RESULT` 宏给出，它会展开成一个函数调用，返回一个整数表
示有多少的测试失败的由 `TAST` 宏调用的函数。

```bash
./tast_program > tast.cout
# after some time for fix
mv tast.cout tast.bout
./tast_program > tast.cout
diff tast.cout tast.bout
```

最后，可将不同时段的测试输出保存文件，用 diff 比较法模拟回归测试。不过此法要注
意的是测试中不宜输出指针地址，因为不同运行时内存地址不可能做到保持一致。另外，
如果测试代码中只用到双参数 `COUT` 测试，一般看统计结果全部通过即可。diff 比较
法更适用于测试代码中大量使用单参数 `COUT` 不能自动判断通过与否的情况。


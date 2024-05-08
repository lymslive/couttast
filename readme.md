# 从 taste 到 test: 轻量级单元测试框架

[English Version](./readme-en.md)

couttast 是一个简单、轻量的 C++ 单元测试库及框架。可用于快速构建基于命令行的可
执行测试程序，却也不仅限于单元测试。详细文档见 [wiki](https://github.com/lymslive/couttast/wiki)。

## 功能概览

* 核心功能仅在单个头文件中：[tinytast.hpp](include/tinytast.hpp) 可当 header-only 库使用。
  + 用统一的 `COUT` 宏打印表达式的值或比较预期结果，提供语句级测试判断；
  + 用 `DEF_TAST` 宏定义单元测试用例，其函数体可简单等效一个 `void()` 函数的写法；
  + 在 `main()` 入口函数中仅需调用 `RUN_TAST` 宏，自动执行测试用例，并返回失败用例数；
  + 空命令行参数时顺序执行所有用例，否则按每个参数执行匹配的测试用例；
  + 命令行参数支持 `--list` 查看已定义的测试用例，`--cout` 可控制输出信息的冗余度。
  + 其他 `*.hpp` 文件扩展 header-only 库的测试功能。
* 高级扩展功能另外打包在静态库中：libcouttast.a 。
  + 提供一个 `main()` 函数，但是弱符号，仍支持用户自定义 `main()` 函数；
  + 提供终端颜色打印功能，进一步增加输出信息的可读性；
  + 增强命令行参数筛选测试用例的功能，支持简单通配符；
  + 支持命令行参数到 `ini` 配置文件的映射，自动读取与程序同名的 `ini` 配置文件；
  + 对于大量测试用例支持多进程模式并行运行测试用例，增加测试效率；
  + 其他利于设计单元测试程序的辅助工具库。
* 依赖低，只依赖 C++ 标准库，核心头文件 `tinytast.hpp` 甚至无需 C++11 功能。
* 利用 `couttast` 书写单元测试用例程序简单、直接、方便。

适用场合：

* 快速验证与测试自己开发的代码功能，并逐步完善为可自动化回归的单元测试，促进可测试
  与可维护的代码编写。
* 以测代学，辅助快速编写简易代码块学习、研究不断进化的 C++ 新标准及未熟悉的隐晦特性。
* 测试想集成使用的第三方库，验证满足自己需求的适合的调用方式。

作者目前主要在 Linux 系统下使用验证，当然 header-only 的 C++ 源码是可移植的。

### 使用方法最简示例

只需在测试文件中包含 `tinytast.hpp` 头文件，然后用 `DEF_TAST` 定义一系列类似
`void()` 函数的代码块，其中可用 `COUT` 输出或比较感兴趣的值，最后在 `main()`
函数中调用 `RUN_TAST` 。

```cpp
#include "tinytast.hpp"

DEF_TAST(test_name, "测试用例说明")
{
    COUT(sizeof(int));
    COUT(sizeof(int), 4);
    COUT(sizeof(int)==4, true);
}

int main(int argc, char** argv)
{
    return RUN_TAST(argc, argv);
}
```

编译、运行大约有如下输出：其中 `COUT` 单参数时只输出结果，双参数时比较结果并给
出正确或通过与否的判据，然后有额外统计汇总信息。

```
## run test_name()
|| sizeof(int) =~? 4
|| sizeof(int) =~? 4 [OK]
|| sizeof(int)==4 =~? ture [OK]
<< [PASS] 0 test_name within 81 us

## Summary
<< [PASS] 1
<< [FAIL] 0
```

可以由多个测试 `*.cpp` 源文件编译链接在一起，当然只能有一个 `main()` 定义。 
如上只用 header-only 库时编译命令大致如下：

```bash
g++ *.cpp -o utMyProgram
# 可能需要用 -I 指定头文件路径
```

如果链接扩展静态库编译测试程序，则可省略写 `main()` 函数，只专注写各个测试用例。
编译命令大致如下：

```bash
g++ *.cpp -lcouttast -o utMyProgram
# 可能需要用 -I 指定头文件存放目录，及 -L 指定静态库存放目录
```

当然在正式项目中建议用专门的构建工具，避免手写编译命令。如果按下节构建方式安装
时，头文件可能被安装至 `/usr/local/include/couttast/` ，则在使用时建议包含子目
录，如下写法：

```cpp
#include "couttast/couttast.h"
/* 或简单头文件 */
#include "couttast/tinytast.hpp"
```

## 构建指引及示例

只用 `tinytast.hpp` 及其他一些 `*.hpp` 头文件的话，其实不需要特别地构建。
本仓库也提供了几个静态库封装、单元测试及一些可运行示例，需要编译构建。

### 用 make 构建

```bash
make
make example
# make test
```

按本仓库主目录上提供的 `makefile` ，直接 `make` 会生成如下几个目录：

* `obj/`： 编译过程的中间目标文件；
* `lib/`： 生成的静态库，其中 `libtinytast.a` 只有一个 `main()` 函数，
  `libcouttast` 包含所有扩展功能；
* `bin/`： 生成的可执行程序，包含可执行的单元测试程序。
* 可执行的示例程序，生成在 `example/` 的各个子目录下。

可选使用 `make test` 运行单元测试，用这个单元测试框架本身测试其主要功能。
可用 `make test -n` 查看如何手动运行单元测试程序。

### 用 CMake 构建

也提供了 `cmake` 构建系统，可用如下标准的构建与安装命令：

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME
make
# make test
# make install
```

用 `cmake` 工具时，按惯例构建工作在单独的 `build/` 子目录下进行。最后一步安装
至个人 `$HOME` 下的几个子目录，`include/couttast` 、`lib/` 与 `bin/` ，这是为
了避免 `/usr/local` 的写权限限制。

如果在前面步骤 `cmake ..` 时不加 `CMAKE_INSTALL_PREFIX` 选项参数，则默认安装至
系统目录 `/usr/local` 。安装至系统目录后，在写实际的单元测试程序时可直接使用，
或集成至其他 CMake 构建的项目。如果安装在 `$HOME` 目录或其他目录，则可能需要在
构建单元测试程序时调整相应的编译参数。

### 构建单元测试程序

有三种方法构建可执行测试程序（命令行程序），可参见 `example/basiccpp-*` 的几个子目录。

1. 自己写 `main` 函数。可以在调用 `RUN_TAST` 之前作些特殊的预处理。
2. 链接静态库 `libtinytast.a` 或 `libcouttast.a`，自己不用写 `main` 函数，
   只关注写测试用例。`libtinytast.a` 的功能相当简单，就只为提供个 `main` 入口函数。
   链接 `libcouttast.a` 库即可使用扩展功能。
3. 将自己的测试用例编译为动态链接库，要求导出 `tast_main()` 函数，该函数一般会
   最终调用 `RUN_TAST` 宏。然后用 `bin/tast_drive` 程序驱动编译的测试动态库，
   将动态库作为第一个命令行参数，剩余参数会传给内部的测试库。

在 `example/` 目录下的 `basiccpp-umain/` 子目录演示了第一种构建法，
`basiccpp-uliba/` 子目录演示了第二种构建法，`basiccpp-udyso/` 演示了第三种构建
法。其中涉及的单元测试用例，都引用 `basiccpp/` 子目录的源文件，所以是相同的单
元测试用例用不同的方法构建测试程序。注意这套单元测试用例有意设置几个失败，以演
示失败时的输出样式，且其中涉及许多 `sizeof` 运算符，在不同系统上编译运行可能有
不同的结果。

按第三种方法，驱动单元测试动态链接库的使用方式如下：

```bash
bin/tast_drive libmytast.so --list
bin/tast_drive libmytast.so [test_name_fileter]
```

注意加载动态库可能要导出环境变量 `$LD_LIBRARY_PATH`，毕竟一般不会将测试动态库
放到系统路径中。不过若用 cmake 构建时，在 `build/` 目录下执行 `./tast_dirve`
可不必添加环境变量 `$LD_LIBRARY_PATH` ，可直接执行：

```bash
./tast_drive example/libtast-basiccpp.so 
example/tast-basiccpp-uliba.exe
example/tast-basiccpp-umain.exe
```

这些构建出来的单元测试程序支持的一些命令行参数，详见后文。

此外，因为动态链接库的方式比较复杂，也更多陷阱，在没特殊需求时，建议使用前两种
方法构建独立可运行的单元测试程序。

### 自定义覆盖 main 函数的弱符号

在 `libcouttast.a` 中定义的 `main()` 入口函数大致如下：

```cpp
#define WEAK_SYMBOL __attribute__((weak))
int WEAK_SYMBOL main(int argc, char* argv[])
{
    return tast::main(argc, argv);
}
```

这意味自己的测试程序在链接 `libcouttast.a` 的同时，如果有需要仍可定义自己的
`main()` 函数，覆盖默认提供的 `main()` 函数。比如在调用 `tast::main()` 之前先
做些其他预处理工作。

另外，基础库 `libtinytast.a` 中仅有的 `main()` 函数也是弱符号，也可以被覆盖。
只不过既然自己写了 `main()` 函数，就没必要再链接 `libtinytast.a` 了，它没有包
含其他许多花哨的扩展功能。

<!-- 内容移至 wiki 重构
## 单元测试库宏定义详解

本单元测试库的功能，主要体现于几个宏的设计上，也是编写各个单元测试用例需要经常
用到的特性元素。

### COUT 语句宏

`COUT` 是本单元测试库核心宏，就是取名自标准库的 `std::cout` 。

* 单参数 `COUT(expr)` ：打印 `expr` 表达式字面量及其值，使其输出信息更有可读意义，
  且更易找到源文件互为参照。支持任意类型，只要其支持 `<<` 操作重载。
* 双参数 `COUT(expr, expect)` ：在单参数的基础上扩展，比较 `expr` 与 `expect`
  的值，如果相等，在行尾打上 `[OK]` 的标记，否则打上 `[NO]` 的标记。比较失败后
  也会再打印 `expect` 的值。表达式与预期值的类型可以不同，只要支持 `==` 操作重载。
* 浮点数三参数 `COUT(expr, expect, limit)`：因为浮点精度原因，直接用 `==` 比较
  浮点数易出问题，所以允许提供额外参数指定精度。

单参数的 `COUT` 也可称为观察宏，双参数（以上）则称为断言宏。
对于断言宏还有两个扩展变种，可选使用。

* `COUTF(expr, expect)`: 只有当断言失败时才输出信息，用于减少成功时的输出信息。
* `COUT_ASSERT(expr, expect)`: 断言失败时会因 `return` 终止当前测试函数，可避
  免因一个关键语句失败导致后面连续失败甚至异常。

### COUT 之于特殊类型

原则一、对于绝大多数类型，以下两个 `COUT` 断言语句结果等效同义：

```cpp
COUT(expr, expect);
COUT(expr == expect, true);
```

原则二、对于绝大多数类型，`COUT` 的单参数与双参数输出内容，除后缀判断标记外相同：

```cpp
COUT(expr);
COUT(expr, expect); // 仅在输出末尾增加 [OK] 或 [NO] 判断标记
```

但对于 C 字符串类型，`const char*` 或 `char*` 时，情况有点特殊。单参数时输出字
符串内容，双参数时输出内容虽然也相同，但比较的是指针值，这是为了满足原则一。
其他类型的指针，输出其十六进制地址值，比较的也是地址值。

若要比较 C 字符串的内容，除了显式用 `strcmp` 外，也可在 `COUT` 中将其中一个参
数转换为 `std::string` ，以便使用 C++ 字符串的 `==` 重载运算符，如：

```cpp
COUT(expr, std::string(expect));
COUT(strcmp(expr, expect), 0);
```

对于浮点数比较，支持如下几种方法使用 `COUT` ：

```cpp
double expr, expect, limit;
COUT(expr, expect, limit);
COUT(abs(expr-expect) <= limit, true);
COUT(expr, expect);
```

其中第一种的三参数形式与第二种的显式比较 `true` 等效。若只有双参数浮点数，采用
计算机表示浮点的最大精度比较，对于简单浮点数，也能通过相等性判断。但在具体业务
中若浮点数结果是从其他复杂的浮点计算而来，可能引入计算误差，理应根据业务需求增
加第三参数表示比较精度。

### COUT 扩展宏

包含 `extra-macros.hpp` ，可使用如下一些扩展宏：

* `COUT_ERROR(n)` 带一个参数，表示当前测试用例检测出 `n` 个错误，即未通过
  `COUT` 断言的语句个数，然后将错误数清零。这适用于设计有意不通过的语句，但又
  要让整个测试用例报告为通过，以便能集成入自动化测试中。
* `COUT_EXCEPTION(statement, message)` 断言执行语句 `statement` 会抛出标准异常
  类 `std::exception` 或其子类，并且异常消息包含 `message` 参数子串。`message`
  认为是字符串 `const char*` 类型，空串 `""` 能匹配任意异常消息，但如果是
  `nullptr` 则预测期望 `statement` 不抛异常。

另外，许多 gtest 风格的断言宏，也可以用 `COUT` 模拟实现，部分示例如：

```cpp
#define EXPECT_EQ(a, b) COUTF(a, b)
#define EXPECT_NE(a, b) COUTF(a != b, true)
...
```

这些模拟实现的宏，仅供参考，并不一定推荐使用。在 `couttast` 库，当然只推荐记一
个断言宏 `COUT` 。

在 `file-compare.h` 头文件中还定义了比较文本文件的宏 `COUT_FILE(file, expect)`
，需要链接 `libcouttast.a` 实现功能。参数是表示文件名的字符串。当只有一个参数
时，打印 `file` 文件内容；当两个参数时，比较 `file` 与 `expect` 的文本行，给出
这两个文件是否相同的判据，不打印原文件内容，只打印第一行差异行。在被测程序有
处理文件相关功能时，可用该宏简化测试。

### DESC 描叙宏

宏 `DESC(msg, ...)`，类似 `printf` 格式打印，但自动加换行符。
可选使用，主要为了改善输出信息的可读性，起到能输出到结果的注释作用。
在一个单元测试比较长时，可适当添加一些 `DESC` 语句，提示每段的测试目的；
也可以在循环中将当前循环次数打印出来，以便观察分析。

### 单元测试用例定义及运行宏

* `DEF_TAST(case_name, desc)`：相当于定义测试函数 `void case_name()` ，并且
  为该测试用例附加一段描叙说明。本质上是定义了一个类及其单例，随后的 `{}` 
  代码块是该类的一个虚函数实现。
* `RUN_TAST()`：自动执行由 `DEF_TAST` 定义的所有测试用例，且按测试名的字典序
  依次执行。可以传入 `(argc, argv)` 命令行参数，以筛选执行特定的测试用例。
* `DEF_TOOL`: 与 `DEF_TAST` 类似，但在默认情况下未指定命令行参数时，不会自动执
  行这类单元测试用例。适于定义那些主要用单参数 `COUT` 观察输出，或运行时间过长
  不适合自动执行的特殊用例。

在定义测试用例时，参数 `case_name` 要求是合法的 C++ 符号（不加双引号），`desc` 是
任意描叙字符串（加双引号）。描叙参数也可省略，但建议加上便于人工管理测试用例。

在运行测试用例时，`RUN_TAST()` 最后会汇总测试结果，并返回是失败用例个数，故可
直接当作 `main` 的返回值， `0` 表示测试成功。整个测试程序的退出码为 `0` 时也
表明失败用例数为 `0` ，测试通过。但退出码非零时，由于系统掩码的关系，不一定表示
失败用例数。

## 命令行参数解析

利用本单元测试库构建出的可执行单元测试程序，本质上是一个命令行程序，它的命令行
位置参数（不带 `--` 引导的选项），用于筛选要执行的单元测试用例。

当没有任何命令行参数时，默认执行所有由 `DEF_TAST` 定义的测试用例，但不执行由
`DEF_TOOL` 定义的用例。且执行顺序按用例名的字典序排列。

### 基本的测试用例筛选功能

在使用基本头文件 `tinytast.hpp` 时，当将多个输入的命令行参数传给 `RUN_TAST()`
时，除特定选项外，将依次对其解析：

* 如果参数完全匹配某个测试名，则只运行该用例。
* 如果参数是 `*.cpp` 文件名及可选附加 `:行号`，则只执行在该文件及某行号之后
  定义的测试用例。
* 查找包含给定参数子串的用例名。

由 `DEF_TOOL` 定义的测试用例，仅当有命令行参数显式匹配时才会执行。

### 扩展的测试试用筛选功能

扩展静态 `libcouttast.a` 提供了更复杂的筛选功能。

* 如果参数仅是单字符，则只运行以该字符开头的用例。
* 如果参数以 `^` 开头或以星号 ` * ` 结尾，则查找特定前缀的用例。
* 如果参数以 `$` 结尾或以星号 ` * ` 开头，则查找特定后缀的用例。
* 以一个短横 `-` 引导的参数，表示不执行匹配的用例，相应的可选的 `+` 前缀表示执
  行匹配的用例。
* 仅一个 `+` 没有词根时，等效于 `--all` 参数，加入所有测试用例，包括 `DEF_TOOL`
  定义的用例。
* 多个参数筛选时，不会重复匹配用例（基本筛选功能不去重）。

这些规则应该符合直觉与方便使用。暂不打算支持完整正则表达式，以保证库的轻量化。

### 精准筛选测试用例及 ini 配置

最后，可以将测试用例全名逐个地写在命令行参数上，如果用例太多，不方便每次写在命
令行上，可以转存到 ini 配置文件中。配置文件与测试程序同名，附加 `.ini` 后缀，
命令行参数一个一行。不要加 `=` ，否则会认为是选项。最好也不要有中括号格式的段名，
或放在第一个段名之前。

故测试用例的执行顺序可预测，也易控制调整。最典型的用法时不带任何参数运行所有测
试用例，或者带上全名单测某一个或几个测试用例。良好的单元测试用例设计，应该使每
一个单元测试可独立运行，不依赖其他用例的特定顺序，重复运行也应没有副作用。

`couttast` 库认读的配置文件只是参考借用 `.ini` 后缀名，与标准 ini 格式不完全兼
容，它本质上是分行且支持注释与空行的可持久化的命令行参数。其主要规则如下：

* 忽略空行及以 `#` 开始的行；
* 带 `=` 的行表示一个命令行选项及其参数，可忽略前缀 `--`；
* 没有参数的选项，不能省略前缀 `--` ，或处理时约定 `=1` 表示选项存在；
* 其他没有 `=` 且无前缀 `--` 的行，表示命令行的一个位置参数；
* 在段名 `[section]` 之下的 `key = val` ，相当于参数 `--section.key=val` 。

例如，有这样的配置文件：

```
# tast_program.ini

case1
case2
casex*

--nocolour

[section]
key = val

[another]
key = 200
```

相当于输入命令行：

```
./tast_program case1 case2 casex* --nocolour --section.key=val --another.key=200
```

本 `couttast` 库只处理位置参数及可识别的选项参数，其他参数在运行任一用例时可通过
`TAST_OPTION` 取得，它是一个字符串类型的 `map` 。如：

```
COUT(TAST_OPTION["section.key"], "val");
COUT(TAST_OPTION["another.key"], "200");
```

如果在命令行与 ini 配置文件中存在同名选项参数，以命令行输入的优先。配置文件的
位置参数接在命令行输入的位置参数之后。

### 基本命令行选项

作为选项的其他命令行参数，都以两个 `-` 开头，如果选项有参数，只能用 `=` 分隔
选项名及其参数，即形如 `--key` 或 `--key=val` 格式。不支持用空格分隔（否则会当
作普通的测试用例参数）。约定统一的选项参数格式也是为实现轻量化考虑。

目前支持的选项有以下几个：

* `--help` : 输出简要帮助信息，可用选项。
* `--list` ：列出所有测试用例名，而不实际运行测试；由 `DEF_TOOL` 定义的用例在
  名字后面附加 `*` 字符以示区分。
* `--List` ：列出所有测试用例名及其描叙等更详细的信息一览表。
* `--cout=fail` ：只输出 `COUT` 断言失败的语句，相当于 `COUTF` 效果。
* `--cout=silent` ：与 `fail` 类似，但输出的信息还更精简，单参数 `COUT` 输出宏
  与纯描叙性的 `DESC` 宏也被沉默，不再有输出。
* `--cout=none` ：所有宏的输出被抑制，只由程序退码是否 0 来判断测试是否通过。当
  然不能抑制在单元测试用例中用户代码手动调用 `printf` 或 `std::cout` 的输出。

另注，`--list` 与 `--help` 一样是短路的，输出信息后直接退出，因此也会忽略后面
提供的测试名筛选参数，而是列出所有测试名。如需查询特定测试名，可管道至 `grep`
或其他的过滤工具。

```
./tast_program --list | grep something
```

### 扩展库的命令行选项

扩展库 `libcouttast.a` 增加的一些功能，也有相应的命令行选项控制。

* `--help` 帮助信息更丰富，同时也以单行紧凑的形式分别列出由 `DEF_TAST` 或
  `DEF_TOOL` 定义的测试用例。
* `--list` 或 `--List` 所列出的用例，可被其他过滤参数影响。
* `--job=` 多进程方式执行测试，指定子进程数量。
* `--nocolour` 与 `--colour` 显式指定是否颜色打印，默认会自动根据是否打印至终
  端判断是否应该启用颜色打印。

## 单元测试程序特殊用法

### 模拟子命令模式的命令行工具

一般地，测试程序中有多个单元测试用例，通过命令行指定用例名可精准选择唯一特定的
测试用例运行，这就相当于包含许多子命令模式的命令行程序，每个单元测试用例相当于
一个子命令。

还可通过双横 `--` 命令行参数或 ini 配置文件向单元测试用例传递额外参数。

* `TAST_OPTION["key"]` 可获取形如 `--key=val` 的命令行选项的相关参数，不存在该
  选项时返回空字符串（`std::string` 类型）。
* 在扩展库 `couttast.h` 中还声明了 `tast::GetOption()` 函数用于获取命令行选项。

这种伪装成子命令的单元测试用例适合用 `DEF_TOOL` 定义，使其在自动化测试中默认不
会被执行到。并且运行时可以考虑加 `--cout=none` 命令行参数完全隐藏 `COUT` 等宏
的输出信息，使子命令自己可能有的输出更干净。

### 测试私有方法及隐藏实现

在某些情况下，可能需要测试或验证私有方法。在 C++ 也有些黑魔法可以实现访问私有
方法，但这里提供一种简单朴素、易懂易操作的方法。就是基于宏定义，但也不能直接
给编译器传全局宏定义如 `g++ -Dprivate=public` ，因为这会导致一些标准库编不过。
所以只宜对局部被测类施以重定义访问权限。

假设开发一个类库，有源文件 `foo.cpp`，头文件 `foo.h` ，为它写单元测试的源文件
叫 `test-foo.cpp` 。那么只要在 `foo.h` 文件中将被测类定义中的 `private` 改为
`private__` 多加两个下划线，并在前面包含 `private-access.hpp` ，然后在测试源文
件 `test-foo.cpp` 中先包含 `couttast.h` 再包含 `foo.h` 即可。大致如下：

```cpp
// foo.h
#include "couttast/private-access.hpp"

class CFoo
{
private__:
    void Fun();
};

// test-foo.cpp
#include `couttast/couttast.h`
#include `foo.h`

DEF_TAST(test_private, "test private method")
{
    CFoo a;
    a.Fun();
}

// compile and link:
g++ test-foo.cpp -lfoo
```

而在正常交付给客户的代码，`CFoo` 类的访问权限控制不受影响，且额外包含的
`private-access.hpp` 入侵几乎是无害的，因为它只有几行类似如下的宏定义：

```cpp
#ifdef HAS_UNIT_TEST
#define private__    public
#else
#define private__    private
#endif
```

另有一种情况，假设在开发库时对接口抽象得很厉害，比如 `foo.h` 头文件中就只暴露
了一个 C 函数给用户调用，而具体实现包括类定义都藏在了 `foo.cpp` 中。用户接口是
极尽优雅了，但自己开发时对实现的正确性若不够自信，想要写单元测试又该如何呢？

可以在测试源文件 `test-foo.cpp` 中直接包含源文件 `foo.cpp` 而不是 `foo.h` ，最
好对开发库中每个源文件都写个对应的测试文件包含源文件，然后在编译测试程序时不要
链接待测开发库。就相当于从完整源文件重新编译单元测试可执行程序，大致如下：

```cpp
// test-foo.cpp
#include "src/foo.cpp"

DEF_TAST(...)

// compile:
g++ test-*.cpp
```

当然，更理想的情况是尽可能写出可测易测的代码，如果只测公开接口就能保证覆盖率与
代码质量那就最好。

### diff 模拟回归测试

使用本单元测试库构建的单元测试程序，默认有丰富的 `COUT` 输出信息，这些输出信息
也形成了测试用例的特征结果。所以可将不同时段的测试输出保存文件，用 diff 比较法
模拟回归测试。

```bash
./tast_program > tast.cout
# after some time for fix ...
mv tast.cout tast.bout
./tast_program > tast.cout
diff tast.cout tast.bout
```

不过此法要注意的是测试中不宜输出指针地址，因为不同运行时内存地址很可能不一致。

另外，如果测试代码中主要用到双参数 `COUT` 测试，一般看统计结果全部通过也就达到
回归测试的目的。即修改程序后，原来通过的单元测试用例集，应该仍保持通过。diff
比较法更适用于测试代码中大量使用单参数 `COUT` 不能自动判断通过与否的情况。

### 性能测试 

在运行完每个单元测试用例之后，除了会打印失败的 `COUT` 断言语句数外，还会打印运
行该用例的耗时，单位微秒。根据该时间报告可大致评估测试用例的运行效率。

在每个单元测试用例代码体中，可以使用两个宏 `TIME_TIC` 与 `TIME_TOC` ，用以限定
为该用例运行计时限定范围。默认省略的情况下，`TIME_TIC` 相当于写在左大括号之后，
而 `TIME_TOC` 相当于写在于右大括号之前。可以根据实际所需显式调整计时起点或（与）
终点。

还有宏 `TIME_TAST` 可以简便快捷地进行性能测试。它可以重复运行由 `DEF_TAST` 定
义的某个测试用例若干次，统计每次的运行时间，并求平均值。每次运行时会调用 `sleep`
暂停 1 秒，最后打印汇报结果。

它可接收 1 至 3 个参数，只有第一个参数用例名是必须的，后面两个有默认值：

* `name`: 测试用例名，即由 `DEF_TAST` （或 `DEF_TOOL`）定义的名称。
* `times`: 重复运行次数，默认 10 次。
* `verbose`: 是否打印每次运行的时间，默认 `true`。但如果启动测试程序时传入了命
  令行参数 `--cout=silent` ，那它也不会打印中间结果。
* `return`: 返回值就是平均时间，单位是微秒。

用于性能测试的用例，虽然（为保持一致）也用 `DEF_TAST` 定义，但由于其设计目的不
同，一般不在用例函数内用双参数 `COUT` 宏比较结果，也就是应该必定 `PASS` 的用例。

而调用 `TIME_TAST` 触发性能测试的语句，可以放在另一个 `DEF_TAST` 的用例中，这
样就可以从命令行参数选定运行该性能测试用例。但如果按默认无命令行参数时，那个被
性能测试的用例也会额外运行一次，不过这一般不是大问题。

在实践中，这种启动性能测试的测试用例一般耗时较长，适合改用 `DEF_TOOL` 定义，与
其他用 `DEF_TAST` 定义的功能测试用例按逻辑分开。因为默认地不执行，只响应显式
参数执行。例如：

```cpp
DEF_TOOL(tictoc, "test tic toc")
{
    usleep(100 * 1000); // pre action
    TIME_TIC; // optional
    for (int i = 0; i < 10; ++i) // simulate complex business
    {
        usleep(200 * 1000);
    }
    TIME_TOC; // optional
    usleep(100 * 1000); // post action
}

DEF_TOOL(avgtime, "test average time")
{
    int64_t avg = TIME_TAST("tictoc", 5);
    COUT(avg);
}
```

甚至还可以将功能测试与性能测试物理分开，编译为不同的可执行测试程序。在性能测
试程序中，可以简单地将若干 `TIME_TAST` 调用放在 `main()` 函数中。因为对于性能
测试，它需要的是运行时间报告，须由人工分析，一般无法预测或比较运行时间多少。

但是可以利用 `TIME_TAST` 的返回值，比较两个算法实现的相对耗时大小，也就能期望
验证一种算法比另一种更快或更慢。
-->

## 开发与贡献指引

开源库，例行欢迎 issue fork pr ，如果觉得此 `couttast` 库对您有用的话。

建议使用 cmake 构建，在 `build/` 目录下执行 `cmake .. && make` 后，请执行
`make test` 确保测试通过。主要是 `utCoutTast` 要通过，它从 `utest/` 子目录的源
文件构建，但在运行时需要读一些测试数据也在 `utest/` 目录下，所以想手动执行它时
要先切换到该目录，然后如下运行：

```bash
cd utest
../build/utCoutTast [--help | --list | 用例名参数]
```

或者在 `build/` 目录直接指定 `--cwd` 选项执行：

```bash
./utCoutTast --cwd=../utest
```

## 历史版本及主要变更记录

### v0.6.02

* 优化测试用例底层管理，从 `map` 改为 `vector` 紧凑存储，且避免复制字符串字面
  量的用例名与文件名。
* 增加对测试单元与测试套件对相关测试用例的组织方案，利于管理大量测试用例。
* 优化头文件组织，划分为三级功能层次。
* 增加对调用外部系列命令的测试功能。
* 增加以数据配置化单元测试的功能。
* 其他增强单元测试及命令行程序的功能。

### v0.6.0

* 增加扩展静态库 `libcouttast.a` ，主要是终端颜色打印、用例筛选增强、自动读取
  ini 配置、多进程并行等功能；
* 原来的只含 `main()` 函数的库由 `libtast_main.a` 改名为 `libtinytast.a` ；
* 单元测试管理类的全局单例宏名由 `_TASTMGR` 改为 `G_TASTMGR` ；
* 为 `COUT` 宏增加一系列 `COUT_BIT_*` 常量控制各类信息的输出，由此 `Print()` 
  方法需要额外的标志参数。
* 移除对 `TAST` 简单宏及 `TAST_RESULT` 的定义。

### v0.2

核心文件：`tinytast.hpp` 。

单头文件简易版。增加了 `DEF_TAST` 宏定义测试用例，在 `main()` 入口函数只需用
`RUN_TAST` 自动调用所有被定义的测试用例。可用命令参数指定筛选运行部分测试用例，
并安排测试顺序。内部采用一个全局单例对象管理所有测试用例，每个 `DEF_TAST` 的用
例除了名称与可选描叙外，函数体可按普通 `void` 函数编写，用 `COUT` 宏比较结果。

提供一些应用示例，及简单驱动程序或封装库，支持 `make` 与 `cmake` 构建。

### v0.1

核心文件：`tast_util.hpp` 。

极简速成版。除了 `COUT` 比较观察宏外，只定义了 `TAST` 宏调用普通 `void` 函数，
用 `TAST_RESULT` 宏汇报结果。没有专门语法定义测试用例，需在 `main()` 函数中手
动调用作为用例的 `void` 函数或用 `TAST` 宏封装调用。

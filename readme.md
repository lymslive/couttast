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

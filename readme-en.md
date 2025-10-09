# From taste to test: a lightweight unit testing framework

couttast is a simple, lightweight C++ unit testing library and framework. Can be used to quickly build command-line-based test program, but not just unit for tests. See [wiki](https://github.com/lymslive/couttast/wiki) for detailed documentation.

## Functional Overview

* Most of the core functionality is in a single header file: [tinytast.hpp](include/tinytast.hpp) can be used as a header-only library.
  + Use a unified `COUT` macro to print the value of the expression or compare the expected result to provide a statement level test judgment;
  + Using `DEF_TAST` macros to define unit test cases, the function body can be simply equivalent to the writing of a `void()` function;
  + Only the macro needs to be called `RUN_TAST` in the `main()` entry function to automatically execute the test case and return the number of failed cases;
  + Execute all test cases in sequence when command line parameters are empty, otherwise execute matching test cases according to each parameter;
  + Command line arguments allow you `--list` to view defined test cases and `--cout` control the redundancy of the output information.
  + And also other `*.hpp` files to extend the header-only library.
* Many of the extensions are packaged in additional static libraries: `libcouttast.a` .
  + Provide a `main()` function, but the symbol is weak. User-defined `main()` function are still supported;
  + Provide terminal color printing to further increase the readability of output information;
  + Enhance the functionality of filtering test cases by command line parameters, and support simple wildcards;
  + Support the mapping from command line arguments to `ini` configuration files, and automatically read the configuration files with the same name as program with `ini` suffix;
  + For a large number of test cases, it supports multi-process mode to run test cases in parallel, which increases the efficiency of testing;
  + Other tools available to assist in the design of unit test procedures.
* Relies on the C + + standard library only, and the core header file `tinytast.hpp` even not requires C++11 support.
* It is simple, direct and convenient to use `couttast` in unit test programming.

Suitable occasions:

* Quickly verify and test the functionality of the code developed by yourself, and gradually improve it into automated regression testing, so as to promote the writing of testable and maintainable code.
* Learning from testing, to assist in quickly writing simple code blocks to learn and study the C + + standard library and unfamiliar obscure features.
* Test the third-party library you want to integrate and verify the appropriate calling method to meet your needs.

At present, the author mainly uses the verification under the Linux system, of course, the C++ source code is portable.

### Simplest Example of How to Use

Simply include the `tinytast.hpp` header file in your test file, and then use some `DEF_TAST` blocks of code that simialrly to a series `void()` of functions that can use `COUT` output or compare expressions or values of interest. Finally call `RUN_TAST` in the `main()` function.

```cpp
#include "tinytast.hpp"

DEF_TAST(test_name, "description for this test case")
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

Compiled and run would have the following output: only the result is output in case of single parameter provided to `COUT` , and the result is compared in case of double parameters then the criterion of correctness or passing is given, and finally there is additional statistical summary information.

```
## run test_name()
|| sizeof(int) =~? 4
|| sizeof(int) =~? 4 [OK]
|| sizeof(int)==4 =~? ture [OK]
<< [PASS] test_name within 81 us

## Summary
<< [PASS] 1
<< [FAIL] 0
```

Multiple test `*.cpp` source files can be compiled and linked together, and of course there can only be one `main()` definition. As above, when only the header-only library is used, the compilation command is roughly as follows:

```bash
g++ *.cpp -o utMyProgram
# May need to specify the header file location with -I
```

If you want to use the extended static library to compile the test program,  you can omit writing the `main()` function and just focus on writing each test case. The compilation command is roughly as follows:

```bash
g++ *.cpp -lcouttast -o utMyProgram
# May need to specify -I and -L for header and library location respectively
```

Of course, in formal projects, it is recommended to use professional build tools to avoid writing compilation commands by hand. If the header files are installed to `/usr/local/include/couttast/`  as constructed in the following section, it is recommended to include a subdirectory when using it, as follows:

```cpp
#include "couttast/couttast.h"
/* or the simple header-only */
#include "couttast/tinytast.hpp"
```

## Building Guidelines and Examples

If you only use `tinytast.hpp` and some other `*.hpp` header files, you don't really need to build them specially. The repository also provides several static library , unit tests, and some runnable examples, which need to be compiled and built.

### Build with Make

```bash
make
make example
# make test
```

Directly running `make` through the `makefile` in the main directory of this repository would generate the extra following directories as:

* `obj/`: The intermediate object file of the compilation process;
* `lib/`: The generated static library, which `libtinytast.a` has only one `main()` function and `libcouttast` contains all the extended functions;
*  `bin/`: Generated executable program, including executable unit test program.
* Executable sample program generated in the various subdirectories of `example/`.

Optionally `make test`, run the unit test, using this unit test framework itself to test its primary functionality. You can `make test -n` view how to run the unit test program manually.

### Build with CMake

A `cmake` build system is also available with standard build and install commands as follows:

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME
make
# make test
# make install
```

When using the `cmake` tools, the build work is done in a separate `build/` subdirectory by convention. The last step is to install to several subdirectories under personal `$HOME`, `include/couttast`, `lib/` and `bin/`, which is to avoid `/usr/local` the write permission restriction.

If the option parameter `CMAKE_INSTALL_PREFIX` is not added in the previous step `cmake..`, it will be installed to the system directory `/usr/local` by default. Once installed in the system directory, it can be used directly when writing actual unit test programs, or integrated into other CMake built projects. If you install in the `$HOME` directory or another directory, you may need to adjust the corresponding compilation parameters when building the unit test program.

### Build Unit Test Program

There are three ways to build an executable test program (command line program), which you can see `example/basiccpp-*` of several subdirectories.

1. Write your own `main` function. You can do some special preprocessing before calling `RUN_TAST`.
2. Link static libraries `libtinytast.a` or `libcouttast.a`, do not write `main` any more, only focus on writing test cases. The `libtinytast.a` is quite simple, just to provide an `main` entry function. Link the `libcouttast.a` library to use the extended functionality.
3. Compile your test cases as a dynamic link library, where required to export the `tast_main()` function, which usually ends up calling the `RUN_TAST` macro. Then use `bin/tast_drive`, the test program driver to take the dynamic library as the first command line parameter, and the remaining parameters will be passed to the internal test library.

The `basiccpp-umain/` subdirectory under the `example/` directory demonstrate the first build way, `basiccpp-uliba/` subdirectory demonstrate the second way, and `basiccpp-udyso/` subdirectory demonstrate the third way. The unit test cases involved all refer to the source files in the `basiccpp/` subdirectory, so from the same unit test cases build the test program in different ways. Note that this set of unit test cases intentionally sets several failures to demonstrate the output style when they fail, and there are many `sizeof` operators involved, which may have different results when compiled and run on different systems.

According to the third method, the driving fro dynamic link library of unit test is used as follows:

```bash
bin/tast_drive libmytast.so --list
bin/tast_drive libmytast.so [test_name_fileter]
```

Note that environment variables `$LD_LIBRARY_PATH` may need to be exported to load dynamic libraries, after all, test dynamic libraries are generally not placed in the system path. However, if cmake is used to build, it can be executed `./tast_dirve` directly in the `build/` directory without adding the environment variables `$LD_LIBRARY_PATH` :

```bash
./tast_drive example/libtast-basiccpp.so 
example/tast-basiccpp-uliba.exe
example/tast-basiccpp-umain.exe
```

Some of the command-line arguments supported by these built unit test programs are detailed later.

In addition, because the dynamic link library approach is more complex and pitfalls, it is recommended to use the first two methods to build a stand-alone unit test program without special requirements.

### Custom Override Weak Symbol for main Function

The entry function `main()` defined in `libcouttast.a` is roughly as follows:

```cpp
#define WEAK_SYMBOL __attribute__((weak))
int WEAK_SYMBOL main(int argc, char* argv[])
{
    return tast::main(argc, argv);
}
```

This means that while linking to `libcouttast.a`, you can still define your own `main()` functions in your test program if necessary, overriding the `main()` function provided by default. For example, do some other preprocessing before calling `tast::main()`.

Also, the only `main()` function in the base library `libtinytast.a` is also weak symbol and can be overridden. It's just that now that you've written the `main()` function, there's no need to link `libtinytast.a` , as it doesn't contain many other fancy extensions.

## Development and Contribution Guidelines

As open source library, welcome to issue fork pr if you find `couttast` useful.

It is recommended to use cmake to build. After executing `cmake.. && make` in the `build/` directory, please execute `make test` to ensure that the test passes, especially for `utCoutTast`. The `utCoutTast` program it is built from the source file in the `utest/` subdirectory, which also contains some test data needs to be read when running, so when you want to execute it manually, you need to switch to the directory first, and then run as follows:

```bash
cd utest
../build/utCoutTast [--help | --list | test_case]
```

Or in the `build/` directory specify the `--cwd` option:

```bash
./utCoutTast --cwd=../utest
```

## Historical Version and Main Change Record

### v0.6.02

* Optimized the core management of test cases, changed from `map` to `vector` for compact storage, and avoided copying string literals of test case name and file name.
* Increase the organization scheme of test units and test suites for related test cases, which is conducive to managing a large number of test cases.
* Optimized the organization of header files, which are divided into three functional levels.
* Added the ability to test the invocation of external shell commands.
* Added the ability to configure unit tests that run with differnt data.
* Other enhancements to unit tests and command line programs.

### v0.6.0

* Add extended static library `libcouttast.a`, mainly including terminal color printing, case filtering enhancement, automatic reading of ini configuration, multi-process parallel and other functions;
* The original `main()` function-only library was renamed to `libtinytast.a` from `libtast_main.a`;
* The global singleton macro name of the unit test management class is changed from `_TASTMGR` to `G_TASTMGR`;
* Add a series `COUT_BIT_*` of constants to the `COUT` macro to control the output of various types of information. Sush `Print()` method requires additional flag parameters.
* Removes the definition of simple macros `TAST` and `TAST_RESULT`.

### v0.2

Core file: `tinytast.hpp`.

Simple version of single header file. `DEF_TAST` macro is added to define test case, and in the `main()` entry function only need `RUN_TAST` to automatically call all the defined test cases. You can specify the command parameters if you want to filter some of the test cases to be run and to arrange the order of the tests. A global singleton object is used internally to manage all test cases. In addition to the name and optional description of each `DEF_TAST` case, the function body can be written as an ordinary `void` function, and used `COUT` to compare the any results.

Provides some application examples, and simple driver or wrapper libraries, support `make`, and `cmake` build.

### v0.1

Core file: `tast_util.hpp`.

Minimalist Express Edition. In addition to `COUT` comparing and observing macros, only `TAST` macro is defined to call ordinary `void` functions, and `TAST_RESULT` macro is used to report summay results. There is no special syntax to define a test case, and you need to manually call the `void` function as a test case in the `main()` function or encapsulate the call with a `TAST` macro.



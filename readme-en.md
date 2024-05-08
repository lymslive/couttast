# From taste to test: a lightweight unit testing framework

couttast is a simple, lightweight C++ unit testing library and framework. Can be used to quickly build command-line-based test program, but not just unit for tests. See [wiki] (https://github.com/lymslive/couttast/wiki) for detailed documentation.

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

<!-- moved to wiki
## Unit Test Library Macro Definition Details

The functionality of this unit test library is mainly based on the design of several macros, and it is also the features that are often used to write each unit test case.

### COUT statement macro

`COUT` is the core macro of this unit test library, named after the standard library `std::cout`.

* Single parameter `COUT(expr)` : Print the `expr` expression literal and its value to make the output information more readable and meaningful, and make it easier to find the source file for mutual reference. Any type is supported, as long as it supports `<<` operator overloading.
* Double parameter `COUT(expr, expect)` : expand on the basis of single parameter, compare the values of `expr` and `expect`, if they are equal, mark `[OK]` at the end of the line, otherwise `[NO]` mark. Value of `expect` will also be reprinted if the comparison fails. The expression can be of a different type than the expected value, as long as `==` operation overloading is supported.
* Floating-point number three parameters `COUT(expr, expect, limit)` : Because of the floating-point precision, it is easy to cause problems by directly comparing floating-point numbers with `==`, so it is allowed to provide additional parameters to specify the precision.

Single-argument macro of `COUT` is also called observation macros, and two-argument (or more) macro is called assertion macro. There are also two extended variants of the assertion macro, which are optional to use as needed.

*  `COUTF(expr, expect)` : Output only when an assertion fails. Used to reduce output on success.
*  `COUT_ASSERT(expr, expect)` : When the assertion fails, the current test function will be terminated by `return` , which can avoid successive failures or even exceptions caused by the failure of a key statement.

### COUT for Special Types

Principle 1. For most types, the results of the following two `COUT` assertion statements are equivalent and synonymous:

```cpp
COUT(expr, expect);
COUT(expr == expect, true);
```

Principle 2. For most types, the output contents of single parameter and double parameter `COUT` are the same except for the suffix judgment mark:

```cpp
COUT(expr);
COUT(expr, expect); // only addition [OK] or [NO] at the end
```

But for C string types, `const char*` or `char*`, the situation is a bit special. The string content is output in case of single parameter, and the output content is the same in case of double parameters, but the pointer value is compared, which is to meet the first principle. Pointers of other types output their hexadecimal address values, and the address values are also compared.

To compare the contents of C strings, as well as  explicitly using `strcmp`, you can also convert one of the arguments to `std::string` in `COUT` to use overloaded operator `==` for C + + strings, such as:

```cpp
COUT(expr, std::string(expect));
COUT(strcmp(expr, expect), 0);
```

For floating-point number comparison, the following methods `COUT` are supported:

```cpp
double expr, expect, limit;
COUT(expr, expect, limit);
COUT(abs(expr-expect) <= limit, true);
COUT(expr, expect);
```

The three-parameter form of the first one is equivalent to the explicit comparison with `true` of the second one. If there is only two parameters of floating point number, use the maximum precision can be represented for comparison of them, and for simple floating point numbers, it can also be judged by equality. However, in a specific business, if the floating-point result is derived from other complex floating-point calculations, any precision error may be introduced, and the third parameter should be added to specify the comparison accuracy according to the business requirements.

### COUT Extended Macro

Include `extra-macros.hpp`, to use some expanded macros as following:

* `COUT_ERROR(n)`: Takes a parameter indicating the number of errors `n` detected by the current test case, that is, the number of statements that fail to pass the `COUT` assertion, and then clears the number of errors. This is useful for designing statements that intentionally fail, but have the entire test case report as a pass so that it can be integrated into the automated test.
* `COUT_EXCEPTION(statement, message)`: Assert that execution of statement `statement` throws the standard exception class `std::exception` or its child classes, and the exception message contains the substring as `message` argument. The `message` is considered to be a string of `const char*` type, an empty string `""` can match any exception message, but if it is `nullptr`, the prediction expects `statement` no exception to be thrown.

In addition, many gtest-style assertion macros can also be implemented by `COUT` emulation, some examples of which are:

```cpp
#define EXPECT_EQ(a, b) COUTF(a, b)
#define EXPECT_NE(a, b) COUTF(a != b, true)
...
```

These analog macros are for reference only and are not necessarily recommended. In the `couttast` library, of course, only one assertion macro `COUT` is recommended.

Macros `COUT_FILE(file, expect)` for comparing text files is defined in the `file-compare.h` header file and need to be linked `libcouttast.a` for implementation. The parameter is a string representing the file name. If there is only one parameter, the file content in `file` will be printed; if there are two parameters, the text line of `file` and `expect` will be compared, and the criterion for judging whether the two files are identical will be given. The original file content will not be printed, and only the first difference line will be printed. This macro can be used to simplify the test when the program under test has the functionality of processing files.

### DESC Describes Macro

Macro `DESC(msg,...)`is similar to `printf` format to print, but with line breaks. Optional, mainly to improve the readability of the output information, it plays the role of annotation that can be output to the result. When a unit test is relatively long, some `DESC` statements can be added appropriately to prompt the test purpose of each segment; you can also print out the current loop times in the loop for easy observation and analysis.

### Unit Test Case Definition and Run Macro

* `DEF_TAST(case_name, desc)`: This is equivalent to defining a test function `void case_name()` and attaching a description to the test case. Essentially, a class and its singleton are defined, and the subsequent `{}` block of code is a virtual function implementation of that class.
*  `RUN_TAST()`: Automatically executes all test cases defined by `DEF_TAST`, in the lexicographic order of the test names. Command line arguments can be passed in `(argc, argv)` to filter out specific test cases for execution.
*  `DEF_TOOL`: Similar to `DEF_TAST`, but does not automatically execute such unit test cases when no command-line arguments are specified by default. It is useful to define special test cases where the output is primarily observed with a single parameter `COUT`, or where the run time is too long for automatic execution.

When defining a test case, the parameter `case_name` is required to be legal C++ symbol (without double quotes) and `desc` arbitrary descriptive strings (with double quotes). Descriptive parameters can also be omitted, but it is recommended to add to facilitate manual management of the test cases.

When running the test case through `RUN_TAST()`, the test results will be summarized at the end, and the number of failed cases will be returned, so it can be directly used as the return value of `main`, `0` indicating that the test is successful. When the exit code of the whole test program is `0`, it just indicates that the number of failed cases is `0`, and the test passes. But when the exit code is not zero. Due to the system mask, it does not necessarily indicate the number of failed cases.

## Command Line Parameter Parsing

An executable unit test program constructed by the unit test library is essentially a command line program, and the command line positional parameter (without a `--` guide option) of the executable is used for screening unit test cases to be executed.

When there are no command-line arguments, all test cases defined by `DEF_TAST` are executed by default, but cases defined by `DEF_TOOL` are not executed. And that ord of execution is in the lexicographic order of the use-case name.

### Basic Test Case Filter Capabilities

When using the basic header file `tinytast.hpp`, when you pass multiple input command-line arguments to `RUN_TAST()`, they are parsed in turn, except for certain options:

* If the parameters exactly match a test name, only that use case is run.
* If the parameters are a `*.cpp` file name and optional appending `:n` as line number, only the test cases defined in the file and after the line number are executed.
* Finds the name of the use case that contains the substring of the given parameter.

The test cases defined by `DEF_TOOL` are executed only when there is an explicit match to any command-line argument.

### Expanded Test Case Filter Capabilities

Extended static `libcouttast.a` provides more sophisticated filtering capabilities.

* If the argument is only a single character, only the use cases that begin with that character are run.
* If the parameter starts with `^` or ends with an asterisk ` * `, looks for a use case for a specific prefix.
* Find a use case for a specific suffix if the parameter ends with `$` or starts with an asterisk ` * `.
* A parameter guided by a dash `-` indicates the cases that does not perform matching, and a corresponding optional `+` prefix indicates the cases do perform matching to be run.
* When only one `+` without any character, it is equivalent to a `--all` parameter, which means to add all test cases, including `DEF_TOOL` defined cases.
* When multiple parameters are filtered, the case will not be matched repeatedly (the basic filtering does not remove duplicates).

These rules should be intuitive and easy to use. Support for full regular expressions is not planned for the time being to keep the library lightweight.

### Precise Filter of Test Cases and ini Configuration

Finally, you can write the full names of the test cases on the command line parameters one by one. If there are too many test cases to write on the command line each time, you can dump them to the ini configuration file. The configuration file has the same name as the test program, appended with a `.ini` suffix, and the command line parameters are one line each. Do not add `=`, otherwise it will be considered as an option. It is also best not to have section name in square brackets or before the first section name.

Therefore, the execution sequence of test cases is predictable and easy to control and adjust. The most typical usage is to run all test cases without any parameters, or to test one or several test cases with a full list. Good unit test case design should make each unit test run independently, independent of the specific order of other test cases, and if run repeatedly without side effects.

The configuration file read by the `couttast` library is only a reference to borrowed the `.ini` suffix name, which is not fully compatible with the standard ini format. It is essentially a line-oriented and supportting comments and blank lines for persistable command line parameters. The main rules are as follows:

* Blank lines and lines beginning with `#` are ignored;
* The line with `=` represents a command line option and its arguments. The prefix `--` is also optional allowed and ignored;
* For options without parameters, the prefix `--` cannot be omitted, or applying the processing convention `=1` indicates that the option exists;
* Any other line  without `=` nor `--` prefixed represent a positional argument to the command line;
* Below the segment name `[section]`, a `key = val` line is equivalent to a parameter `--section.key=val`.

For example, there is a configuration file:

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

Equivalent to entering a command line:

```
./tast_program case1 case2 casex* --nocolour --section.key=val --another.key=200
```

This `couttast` library only deals with positional parameters and recognizable option parameters. Other parameters can be obtained through `TAST_OPTION` when running any use case. It is a `map` of string type. Such as:

```
COUT(TAST_OPTION["section.key"], "val");
COUT(TAST_OPTION["another.key"], "200");
```

If an option parameter with the same name exists on the command line and in the ini configuration file, the one entered on the command line takes precedence. The positional parameters of the configuration file follow the positional parameters entered on the command line.

### Basic Command Line Option

All other command line arguments for an option begin with two `-`. If the option has an argument, the option name and its argument can only be separated by a `=` form such as `--key` or `--key=val`. Space separation is not supported (otherwise it is treated as a normal test case argument). The agreed unified option parameter format is also for the purpose of achieving lightweight.

The following options are currently supported:

*  `--help` : Output brief help information, and available options.
*  `--list` : List all test case names without actually running the tests; use cases from `DEF_TOOL` are distinguished by appending a `*` character to the names.
*  `--List` : List more detailed information such as the names of all test cases and their descriptions.
*  `--cout=fail` : `COUT` only outputs the statement where the assertion failed, which is equivalent to `COUTF` in effect.
*  `--cout=silent` : Similar to `fail`, but with more concise output information. Single-argument `COUT` output macros and purely `DESC` descriptive macros are also silenced, with no output.
* `--cout=none` : The output of all macros is suppressed, and the test is passed only by whether the exit code is 0 or not. You certainly can't suppress manual calls `printf` or `std::cout` output from user code in unit test cases.

Note that, `--list` likes `--help`, is short-circuited and exits directly after outputting information, so it also ignores the test name filter parameters provided later and lists all test names instead. If you need to query a specific test name, you can pipe to `grep` or other filtering tools.

```
./tast_program --list | grep something
```

### Command Line Options from Extension Library

Some functionalities added by the extension library `libcouttast.a` are also controlled by corresponding command line options.

*  `--help`: The help is more informative and also lists the test cases defined by `DEF_TAST` or `DEF_TOOL` separately in a single, compact line.
*  `--list` or `--List`: The listed use cases, can be affected by other filtering parameters.
* `--job=`: Execute the test in a multi-process manner, specifying the number of child processes.
*  `--nocolour` or `--colour`: Used to explicitly specify color printing or not, whidh default is automatically determine whether color printing should be enabled according to whether to print to the terminal.

## Special Usage of Unit Test Program

### Command Line Tool that Emulates Subcommand Mode

Generally, there are multiple unit test cases in a test program, and a unique specific test case can be precisely selected to run by specifying the case name on the command line, which is equivalent to a command line program containing many subcommand patterns, and each unit test case is considered as a subcommand.

You can also pass additional parameters to an unit test case through a double-horizontal `--` command-line argument or an ini configuration file.

*  `TAST_OPTION["key"]` Gets the associated argument for a command-line option of the form `--key=val`. If the option does not exist, an empty string (`std::string` type) is returned.
* `tast::GetOption()` Function is also declared in the extension library `couttast.h` to get a command-line option.

This unit test case, disguised as a subcommand, is suitably `DEF_TOOL` defined so that it is not executed by default in an automated test. And when running, you can consider adding `--cout=none` command line parameter to completely hide the output information from the `COUT` and other macros, so that the output of the subcommand itself may be cleaner.

### Testing Private Methods and Hidden Implementations

In some cases, you might want to test or validate a private method. There is also some black magic in C++ to access private methods, but here is a simple, easy to understand and easy to operate method. It is based on macro definitions, but it is not good idea to pass global macro definitions as `g++ -Dprivate=public` directly to the compiler, because this will cause some standard libraries to fail. Therefore, it is only appropriate to redefine access permissions for a local class under test.

Suppose you develop a class library with source files `foo.cpp` and header files `foo.h`, and write unit tests for it in a source file called `test-foo.cpp`. Then just change the `private` permission of the tested class in the `foo.h` file to `private__`, that append two more underscores and include `private-access.hpp` at the front. Then include `couttast.h` and then include `foo.h` in the test source file `test-foo.cpp`. Roughly as follows:

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

In the product code normally delivered to the customer, the access control of the `CFoo` class is not affected, and the `private-access.hpp` extra intrusion is almost harmless, because it only has a few lines of macro definitions like the following:

```cpp
#ifdef HAS_UNIT_TEST
#define private__    public
#else
#define private__    private
#endif
```

In another scenes, it is assumed that the interface is heavily abstracted when developing a library. For `foo.h` example, only a C function is exposed in the header file to the user to call, while the specific implementation, including any class definition, is hidden in the `foo.cpp`. The user interface is extremely elegant, but what if you don't have enough confidence in the correctness of the implementation when you develop it yourself and want to write unit tests?

You can directly include the source `foo.cpp` file in the test source `test-foo.cpp` file instead of `foo.h`. It is better to write a corresponding test file including the source file for each source file in the development library, and then do not link to the test development library when compiling the test program. It is equivalent to recompiling the unit test executable from the complete source file, roughly as follows:

```cpp
// test-foo.cpp
#include "src/foo.cpp"

DEF_TAST(...)

// compile:
g++ test-*.cpp
```

Of course, it is more desirable to write code that is as testable as possible, and it is best if coverage and code quality can be guaranteed by only testing the public interface.

### Simulated Regression Testing with Diff

The unit test program constructed by this unit test library has abundant `COUT` output information by default, and the output information also forms the characteristic result of the test case. Therefore, the test output of different time periods can be saved as a file, and the diff comparison method can be used to simulate the regression test.

```bash
./tast_program > tast.cout
# after some time for fix ...
mv tast.cout tast.bout
./tast_program > tast.cout
diff tast.cout tast.bout
```

However, this method should be noted that the pointer address should not be output in the test, because the memory address is likely to be inconsistent at different runtimes.

In addition, if double parameter `COUT` test is heavily used in the test code, the statistical results whether all are passed can also achieve the purpose of regression testing. That is, after modifying the program, the original set of unit test cases that passed should still pass. The diff comparison method is more suitable for the situation that a large number of single parameters `COUT` are used in the test code and can not automatically judge whether it passes or not.

### Performance Testing

After each unit test case is run, in addition to the number of `COUT` failed assertion statements, the elapsed time in microseconds to run the case is also printed. Based on the time report, the running efficiency of the test cases can be roughly evaluated.

In the body of each unit case, you can use two optional macros `TIME_TIC`, and `TIME_TOC`, to bound the time bounds for the run of that case. When omitted by default, `TIME_TIC` it is equivalent to writing after the opening brace and `TIME_TOC` before the closing brace. You can explicitly adjust the start or (and) end of the timing according to your actual needs.

There are also macro `TIME_TAST` for quick and easy performance testing. It can repeatedly run a test case defined by `DEF_TAST` several times, count the running time of each run, and calculate the average value. Each run will call `sleep` pause for 1 second, and finally print the report results.

It can take from 1 to 3 parameters, with only the first parameter, the use-case name, being required, and the next two having default values:

*  `name` : The test case name, which is the name defined by `DEF_TAST` (or `DEF_TOOL`).
*  `times` : The number of repetitions, 10 by default.
*  `verbose` : Whether to print the time of each run. Default `true`. However, if the command line arguments `--cout=silent` are passed in when the test program is started, it will not print the intermediate results.
*  `return` : The return value is the average time in microseconds.

Use cases used for performance testing are also `DEF_TAST` defined (for consistency), but because of their different design purposes, it is generally not necessary to use two-parameter `COUT` macros to compare results within the function, that is, such use cases that should be consider as `PASS` .

The statement invoking `TIME_TAST` that triggered the performance test can be placed in another `DEF_TAST` case, so that the performance test case can be selected to run from the command line parameters. However, if the default is no command line parameters, the performance test case will also run an extra time, but this is generally not a big problem.

In practice, such test cases for initiating performance testing are generally time-consuming, and it is appropriate to use `DEF_TOOL` definitions instead, logically separated from other `DEF_TAST` defined functional test cases. Becouse it dose not execute by default, only response to explicit arguments. For example:

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

Functional tests can even be physically separated from performance tests and compiled into different executable test programs. In a performance test program, you can simply place several `TIME_TAST` calls in the `main()` function. Because for performance testing, what it needs is a running time report, which must be analyzed manually, and it is generally impossible to predict or compare the running time.

However, you can use the return value of `TIME_TAST` to compare the relative time consumption of the two algorithms, and you can expect to verify that one algorithm is faster or slower than the other.
-->

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



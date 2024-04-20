/**
 * @file couttast.h
 * @author tanshuil
 * @date 2024-03-24
 * @brief The main header file to use the full featured couttast library.
 * @note The `main()` function has defined in this libarry as week symbol,
 * which means you can still override the `main()` function if needed.
 * @note If any, will read the `*.in` file with the same name as program.
 * */
#ifndef COUT_TAST_H__
#define COUT_TAST_H__

#include "tinytast.hpp"

namespace tast
{

/// The main entry point for the core test program.
/// A custom main() function with strong symbol can be provided to override
/// the default `::main` that only call this `tast::main()`.
int main(int argc, char* argv[]);

} // end of namespace tast

/// Tell the compiler mean to compile unit test program.
/// May useful in conditional compile.
#define HAS_UNIT_TEST 1

/// Make the function which embrace this marco only be called once.
#define CALL_FUNCTION_ONCE(...) \
    static bool once_ = false; \
    if (once_) { return __VA_ARGS__; } \
    once_ = true

/// Rename TAST to TEST, for compatibility.
#define DEF_TEST(...) DEF_TAST(__VA_ARGS__)
#define RUN_TEST(...) RUN_TAST(__VA_ARGS__)

#endif /* end of include guard: COUT_TAST_H__ */

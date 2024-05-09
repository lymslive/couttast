/**
 * @file couttast.h
 * @author tanshuil
 * @date 2024-03-24
 * @brief The main header file to use the full featured couttast library.
 * @note The `main()` function has defined in this libarry as week symbol,
 * which means you can still override the `main()` function if needed.
 * @note If any, will read the `*.ini` file with the same name as program.
 * */
#ifndef COUT_TAST_H__
#define COUT_TAST_H__

#include "tinytast.hpp"
#include "tinyini.h"
#include "agent.h"
#include "xmltast.h"
#include "systemtast.h"
#include "file-compare.h"

/// The namespace for core functionality of couttast library. 
namespace tast
{

/// The main entry point for the core test program.
/// A custom main() function with strong symbol can be provided to override
/// the default `::main` that only call this `tast::main()`.
int main(int argc, char* argv[]);

} // end of namespace tast

/** 
 * @mainpage CoutTast
 * Couttast is a simple, lightweight C++ unit testing library and framework. 
 * It can be used to quickly build command-line-based test program, but not just unit for tests. See [wiki](https://github.com/lymslive/couttast/wiki) for detailed documentation in Chinese, where this documentation generated from comment by doxygen is in English.
 * */
#endif /* end of include guard: COUT_TAST_H__ */

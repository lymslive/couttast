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
#include "xmltast.h"
#include "systemtast.h"
#include "file-compare.h"

namespace tast
{

/// The main entry point for the core test program.
/// A custom main() function with strong symbol can be provided to override
/// the default `::main` that only call this `tast::main()`.
int main(int argc, char* argv[]);

} // end of namespace tast

#endif /* end of include guard: COUT_TAST_H__ */

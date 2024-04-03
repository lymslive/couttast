/** 
 * @file filter.h
 * @author lymslive
 * @date 2023-06-13
 * @brief Filter test cases by command line options and arguments.
 * */
#ifndef FILTER_H__
#define FILTER_H__

#include "format.h"
#include "tinyini.h"

namespace tast
{

/** Filter test cases by comand line arguemnts.
 * @param [IN] tastInput, the input test cases.
 * @param [IN-OUT] tastOutput, the output result test cases.
 * @param [IN] cli, struct for cli argument, may merged with ini.
 * @details Rules to deal with each argument:
 *   1. match a test case name as a whole;
 *   2. `filename.cpp:line` pattern to match all test cases in the `file` and
 *     below `line`, but `line` is optional and default to 0;
 *   3. single character match those test cases that begin with this char;
 *   4. `^arg` or `arg*` match test cases that begin with `arg`;
 *   5. `arg$` or `*arg` match test cases that end with `arg`;
 *   6. normal `arg` match test cases that contains `arg`;
 *   7. the argument to add test cases can optional add a `+` prefix,
 *     and a single `+` char means match any test case.
 *   8. the argument prefix with `-` means subtract the matched test cases,
 *     but requires at least two characters to distinguish from short option;
 *   9. option `--all` add all test case include tools from `DEF_TOOL`.
 *   10. option `--notool` means not include tool cases even match some argument;
 *   11. option `--random` means randomly reorder the list if more than 2 length;
 *
 * @note The output is vector that keep the order specified in command line
 *  order, while each test case only output onces.
 * @note If there is no postion argument to add any test case, then will
 *  add all auto-run test cases from input, except `--all` option specified.
 * @note The basic argument parse in `tinytast.hpp` roughly apply only 1,2,6,
 *  where may repeat run a test case if it match multiple arguments.
 * */
void filter_tast(const TastMap& tastInput, TastList& tastOutput, const CTinyIni& cli);

/// Filter test case by config already parsed from command line arguments.
void filter_tast(const TastMap& tastInput, TastList& tastOutput, const CTastConfig& cfg);

/// Reorder randomly for a test case list, in [begin, end).
void random_tast(TastEntry* begin, TastEntry* end);

/// The min amount of test cases to enable random order.
const int MINLIST_TO_RANDOM = 2;

} // end of namespace tast

#endif /* end of include guard: FILTER_H__ */

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

#define HAS_UNIT_TEST 1

namespace tast
{

/// Type for command line argument and option.
typedef std::map<std::string, std::string> Options;
typedef std::vector<std::string> Arguments;

/// Get the global cli option and argument, stored in `CTastMgr` instance.
bool GetOption(const std::string& key, std::string& val);
bool GetOption(const std::string& key, int& val);
const std::vector<std::string>& GetArguments();

} // end of namespace tast

/// Make the function which embrace this marco only be called once.
#define CALL_FUNCTION_ONCE(...) \
    static bool once_ = false; \
    if (once_) { return __VA_ARGS__; } \
    once_ = true

/// Rename TAST to TEST, for compatibility.
#define DEF_TEST(...) DEF_TAST(__VA_ARGS__)
#define RUN_TEST(...) RUN_TAST(__VA_ARGS__)

#endif /* end of include guard: COUT_TAST_H__ */

/**
 * @file file_compare.h
 * @author lymslive
 * @date 2024-03-25
 * @brief Implement COUT_FILE macro to compare tows text files.
 * */
#ifndef FILE_COMPARE_H__
#define FILE_COMPARE_H__

#include "tinytast.hpp"

namespace tast
{
namespace macro
{

/// Implement COUT_FILE(file)
bool cout_file(const tast::CLocation& location, const std::string& file);

/// Implement COUT_FILE(file, expect)
bool cout_file(const tast::CLocation& location, const std::string& file, const std::string& expect);

}
} // end of tast::macro::

/// Print out the content lines of file if only one argument, or
/// compare the lines of tow fiels and report the first difference.
#define COUT_FILE(file, ...) tast::macro::cout_file(SRC_LOCATION, file, ## __VA_ARGS__)

#endif /* end of include guard: FILE_COMPARE_H__ */

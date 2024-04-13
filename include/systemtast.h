/**
 * @file systemtast.h
 * @author lymslive
 * @date 2024-04-09
 * @brief Implement COUT_SYSTEM macro to execute and verify extern command.
 * */
#pragma once
#ifndef SYSTEMTAST_H__
#define SYSTEMTAST_H__

#include "tinytast.hpp"

namespace tast
{

/// Constant flag to indicate how to match and check two strings. 
enum EMatchFlag
{
    STRING_MATCH_TRIM = 0, //< trim space in both end before compare string.
    STRING_MATCH_EQUAL,    //< exactly compare by == operator for std::string.
    STRING_MATCH_SUBSTR,   //< check by std::string::find() method.
    STRING_MATCH_WILD,     //< as shell wild, '?' match single, '*' match any.
    STRING_MATCH_REGEXP,   //< check by std::regex_search() method
};

namespace macro
{

/** Implement COUT_SYSTEM(command, ...).
 * @param [IN] location: present source location from macro.
 * @param [IN] command: the shell command to execute.
 * @param [IN] exitCode: the expected exit code, default to 0.
 * @param [IN] output: the expect output substr or pattern.
 * @param [IN] flagMatch: the method to match expected output, 
 * @refer to EMatchFlag, and default to TRIM.
 * */
bool cout_system(const tast::CLocation& location, const std::string& command);
bool cout_system(const tast::CLocation& location, const std::string& command, int exitCode);
bool cout_system(const tast::CLocation& location, const std::string& command, const std::string& output, int flagMatch = 0);
bool cout_system(const tast::CLocation& location, const std::string& command, int exitCode, const std::string& output, int flagMatch = 0);

}
} /* tast::macro */ 

/// Execute extern command, print its exit code and standard output, and then
/// verify the exit code or output if additional expected argument provided.
#define COUT_SYSTEM(command, ...) tast::macro::cout_system(SRC_LOCATION, command, ## __VA_ARGS__)

#endif /* end of include guard: SYSTEMTAST_H__ */

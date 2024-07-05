/** 
 * @file couthex.hpp
 * @author lymslive
 * @date 2024-07-05
 * @brief Cout binary data in hex format by COUT_HEX(val[,expect]) macro.
 * */
#pragma once
#ifndef COUTHEX_HPP__
#define COUTHEX_HPP__

#include "tinytast.hpp"
#include "logprint.hpp"

namespace tast
{
namespace macro
{

inline
bool cout_hex(const tast::SLocation& location, const char* expr, const std::string& str)
{
    std::string output = utd::strhex(str.c_str(), str.size());
    return tast::CStatement(location, expr).cout(_TASTMGR, output);
}

inline
bool cout_hex(const tast::SLocation& location, const char* expr, const std::string& str, const std::string& expect)
{
    std::string output = utd::strhex(str.c_str(), str.size());
    return tast::CStatement(location, expr).cout(_TASTMGR, output, expect);
}

template <typename T>
bool cout_hex(const tast::SLocation& location, const char* expr, const T& val)
{
    std::string output = utd::strhex(val);
    return tast::CStatement(location, expr).cout(_TASTMGR, output);
}

template <typename T>
bool cout_hex(const tast::SLocation& location, const char* expr, const T& val, const std::string& expect)
{
    std::string output = utd::strhex(val);
    return tast::CStatement(location, expr).cout(_TASTMGR, output, expect);
}

} /* macro */ 
} /* tast */ 

#define COUT_HEX(val, ...) tast::macro::cout_hex(SRC_LOCATION, #val, val, ## __VA_ARGS__)

#endif /* end of include guard: COUTHEX_HPP__ */

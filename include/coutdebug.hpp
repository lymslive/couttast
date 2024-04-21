/**
 * @file coutdebug.hpp
 * @author lymslive
 * @date 2024-04-21
 * @brief Adopt COUT in develop code base by log utility.
 * */
#pragma once
#ifndef COUT_DEBUG_HPP__
#define COUT_DEBUG_HPP__


#ifdef _DEBUG

#include "logprint.hpp"

#ifndef COUT
#define COUT(var) COUT_DBG(#var " = %s", var)
#endif
#ifndef COUT_VAR
#define COUT_VAR(var) COUT_DBG(#var " = %s", var)
#endif

#endif

#ifndef COUT
#define COUT(var) 
#endif
#ifndef COUT_VAR
#define COUT_VAR(var)
#endif

#ifndef COUT_DBG
#define COUT_DBG(format, ...) 
#endif

#endif /* end of include guard: COUTDEBUG_HPP__ */

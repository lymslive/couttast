/**
 * @file couttast.hpp
 * @author lymslive
 * @date 2024-04-20
 * @brief Collect most of the head-only header files.
 * */
#pragma once
#ifndef COUT_TAST_HPP__
#define COUT_TAST_HPP__

#include "tinytast.hpp"
#include "classtast.hpp"
#include "tastargv.hpp"
#include "coutstd.hpp"
#include "gtest-macros.hpp"
#include "extra-macros.hpp"

/// Rename TAST to TEST, for compatibility.
#define DEF_TEST(...) DEF_TAST(__VA_ARGS__)
#define RUN_TEST(...) RUN_TAST(__VA_ARGS__)

#endif /* end of include guard: COUT_TAST_HPP__ */

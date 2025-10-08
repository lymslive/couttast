/**
 * @file test-tinyini.hpp
 * @brief Common declarations for tinyini test cases
 */
#ifndef TEST_TINYINI_HPP__
#define TEST_TINYINI_HPP__

#include "tinyini.h"
#include "coutstd.hpp"

namespace tast
{
    inline std::ostream& operator<<(std::ostream& os, const CTinyCli& cli)
    {
        os << "...\n";
        os << "option: " << cli.m_mapOption << "\n";
        os << "argument: " << cli.m_vecArg;
        return os;
    }
}

#endif /* end of include guard: TEST_TINYINI_HPP__ */
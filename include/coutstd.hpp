/** 
 * @file coutstd.hpp
 * @author lymslive
 * @date 2024-05-08
 * @brief Implement the `<<` operator for some of the mose often used
 * container in `std::`.
 * @note Required c++11 or above, and the element type in continer should
 * support `<<` operator.
 * */
#ifndef COUTSTD_HPP__
#define COUTSTD_HPP__

#include <ostream>
#include <vector>
#include <map>
#include <set>

namespace std {
    template <typename valueT, typename valueU>
    std::ostream& operator<<(std::ostream& os, const std::pair<valueT, valueU>& item)
    {
        os << "(" << item.first << "," << item.second << ")";
        return os;
    }

    template <typename valueT>
    std::ostream& operator<<(std::ostream& os, const std::vector<valueT>& vec)
    {
        os << "[";

        bool first = true;
        for (auto& item : vec)
        {
            if (!first)
            {
                os << ",";
            }
            os << item;
            first = false;
        }

        os << "]";
        return os;
    }

    template <typename keyT, typename valueT>
    std::ostream& operator<<(std::ostream& os, const std::map<keyT, valueT>& map)
    {
        os << "{";

        bool first = true;
        for (auto& item : map)
        {
            if (!first)
            {
                os << ",";
            }
            os << item.first << ":" << item.second;
            first = false;
        }

        os << "}";
        return os;
    }

    template <typename keyT>
    std::ostream& operator<<(std::ostream& os, const std::set<keyT>& set)
    {
        os << "{";

        bool first = true;
        for (auto& item : set)
        {
            if (!first)
            {
                os << ",";
            }
            os << item;
            first = false;
        }

        os << "}";
        return os;
    }
}

#endif /* end of include guard: COUTSTD_HPP__ */

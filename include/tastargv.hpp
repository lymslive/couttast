/**
 * @file tastargv.hpp
 * @author lymslive
 * @date 2024-04-20
 * @brief CTinyCli proxy and macros to access the cmdline arguments options.
 * */
#pragma once
#ifndef TASTARGV_HPP__
#define TASTARGV_HPP__

#include "tinytast.hpp"

namespace tast
{
    
typedef std::vector<std::string> CliArgument;
typedef std::map<std::string, std::string> CliOption;

/// Wrap the CTinyCli object and extend some functions.
class CTastArgv
{
    CTinyCli* w_pTinyCli;

public:
    CTastArgv(CTinyCli* pTinyCli) : w_pTinyCli(pTinyCli) {}

    CliArgument& Argument() const
    {
        return w_pTinyCli->m_vecArg;
    }

    CliOption& Option() const
    {
        return w_pTinyCli->m_mapOption;
    }
    
    static
    const std::string& NoneValue()
    {
        static std::string empty;
        return empty;
    }

    const std::string& GetValue(size_t index) const
    {
        if (index < w_pTinyCli->m_vecArg.size())
        {
            return w_pTinyCli->m_vecArg[index];
        }
        return NoneValue();
    }

    const std::string& GetValue(const std::string& key) const
    {
        std::map<std::string, std::string>::iterator it = w_pTinyCli->m_mapOption.find(key);
        if (it != w_pTinyCli->m_mapOption.end())
        {
            return it->second;
        }
        return NoneValue();
    }

    const std::string& operator[](size_t index) const
    {
        return GetValue(index);
    }

    const std::string& operator[](const std::string& key) const
    {
        return GetValue(key);
    }

    bool HasKey(size_t index) const
    {
        return GetValue(index).empty() == false;
    }

    bool HasKey(const std::string& key) const
    {
        return GetValue(key).empty() == false;
    }

    bool BindValue(std::string& val, const char* name, const char* altName = NULL) const
    {
        if (altName != NULL)
        {
            name = altName;
        }

        const std::string& arg = GetValue(name);
        if (arg.empty())
        {
            return false;
        }

        val = arg;
        return true;
    }

    bool BindValue(std::string& val, const char* name, size_t index) const
    {
        const std::string& arg = GetValue(index);
        if (arg.empty())
        {
            return false;
        }

        val = arg;
        return true;
    }

    bool BindValue(std::string& val, size_t index) const
    {
        return BindValue(val, NULL, index);
    }

    template <typename numberT>
    bool BindValue(numberT& val, const char* name, const char* altName = NULL) const
    {
        if (altName != NULL)
        {
            name = altName;
        }

        const std::string& arg = GetValue(name);
        if (arg.empty())
        {
            return false;
        }

        std::stringstream iss(arg);
        iss >> val;
        return true;
    }

    template <typename numberT>
    bool BindValue(numberT& val, const char* name, size_t index) const
    {
        const std::string& arg = GetValue(index);
        if (arg.empty())
        {
            return false;
        }

        std::stringstream iss(arg);
        iss >> val;
        return true;
    }

    template <typename numberT>
    bool BindValue(numberT& val, size_t index) const
    {
        return BindValue(val, NULL, index);
    }
};

} /* tast:: */ 

/// Access object for global arguments, can use operator[] and methods.
/// operator[key] get option and operator[index] get positional arguemnt.
#define TAST_ARGV tast::CTastArgv(G_TASTMGR)

/// Bind vairalbe to option by name or positional argument by index.
#define BIND_ARGV(val, ...) tast::CTastArgv(G_TASTMGR).BindValue(val, #val, ## __VA_ARGS__)

#endif /* end of include guard: TASTARGV_HPP__ */

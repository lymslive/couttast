#include "filter.h"

#include <random>
#include <algorithm>
#include <unordered_set>

namespace tast
{
    
/// Functor to filter input to output by config.
struct CFilterTast
{
    const TastPool& input;          //< input test case collection
    TastList& output;               //< output test case list
    const CTastConfig& config;      //< the parsed config from cli
    std::unordered_set<TastEntry> keys; //< alread outputed test case

    std::vector<std::string> subs;  //< arguments to subtract cases -xxx
    bool optAll = false;            //< --all
    bool optNoTool = false;         //< --notool

    CFilterTast(const TastPool& pool, TastList& filter, const CTastConfig& config_in)
        : input(pool), output(filter), config(config_in)
    {}

    static
    bool MatchWild(const char* name, const std::string& arg, char wild)
    {
        const char* find = ::strstr(name, arg.c_str());
        if (wild == '%')
        {
            return find != nullptr;
        }
        else if (wild == '^')
        {
            return find == name;
        }
        else if (wild == '$')
        {
            // refer MatchEnd
        }
        return false;
    }

    static
    bool MatchEnd(const char* name, int nameLen, const std::string& arg)
    {
        return nameLen >= arg.size() && 0 == ::memcmp(name + nameLen - arg.size(), arg.c_str(), arg.size());
    }

    static
    char MatchArg(const std::string& arg, TastEntry item)
    {
        // full match name
        if (item->EqualName(arg))
        {
            return '=';
        }

        // match file:line
        size_t idot = arg.rfind(".cpp");
        if (idot != std::string::npos)
        {
            std::string file = arg.substr(0, idot + 4);
            int line = 0;
            size_t icolon = arg.rfind(":");
            if (icolon != std::string::npos)
            {
                line = atoi(arg.c_str() + icolon + 1);
            }
            return (item->MatchFile(file, line) ? 'f' : 0);
        }

        // match wild ^$*
        size_t iend = arg.size() - 1;
        const char* name = item->m_name;
        int nameLen = item->m_nameLen;
        if (arg.size() == 1)
        {
            return MatchWild(name, arg, '^') ? '^' : 0;
        }
        else if (arg[0] == '^')
        {
            return MatchWild(name, arg.substr(1), '^') ? '^' : 0;
        }
        else if (arg[iend] == '*')
        {
            return MatchWild(name, arg.substr(0, iend), '^') ? '^' : 0;
        }
        else if (arg[iend] == '$')
        {
            return MatchEnd(name, nameLen, arg.substr(0, iend)) ? '$' : 0;
        }
        else if (arg[0] == '*')
        {
            return MatchEnd(name, nameLen, arg.substr(1)) ? '$' : 0;
        }
        else
        {
            return MatchWild(name, arg, '%') ? '%' : 0;
        }

        return 0;
    }

    bool MatchSub(TastEntry item)
    {
        for (auto& sub : config.subs)
        {
            if (!sub.empty() && MatchArg(sub, item))
            {
                return true;
            }
        }
        return false;
    }

    void AddTast(TastEntry item)
    {
        if (item == nullptr)
        {
            return;
        }
        if (keys.count(item) > 0)
        {
            return;
        }
        if (config.notool && item->m_autoRun == false)
        {
            return;
        }
        if (!MatchSub(item))
        {
            output.push_back(item);
            keys.insert(item);
        }
    }

    void AddAll()
    {
        for (auto& item : input)
        {
            if (item.m_autoRun || config.all)
            {
                AddTast(&item);
            }
        }
    }

    void AddArg(const std::string& arg)
    {
        for (auto& sub : config.subs)
        {
            if (sub == arg || arg.find(sub) != std::string::npos)
            {
                return;
            }
        }

        TastList filter;
        for (auto& item : input)
        {
            char match = MatchArg(arg, &item);
            if (match == '=')
            {
                return AddTast(&item);
            }
            else if (match != 0)
            {
                filter.push_back(&item);
            }
        }

        for (auto& item : filter)
        {
            AddTast(item);
        }
    }

    void Run()
    {
        if (config.adds.empty())
        {
            return AddAll();
        }

        for (auto& arg : config.adds)
        {
            if (arg == "+")
            {
                AddAll();
            }
            else if (arg[0] == '+')
            {
                AddArg(arg.substr(1));
            }
            else
            {
                AddArg(arg);
            }
        }
    }
};

/* ---------------------------------------------------------------------- */

void filter_tast(const TastPool& tastInput, TastList& tastOutput, const CTinyIni& cli)
{
    CTastConfig cfg;
    cfg.ParseCli(cli);
    return filter_tast(tastInput, tastOutput, cfg);
}

void filter_tast(const TastPool& tastInput, TastList& tastOutput, const CTastConfig& cfg)
{
    if (tastInput.empty())
    {
        return;
    }

    CFilterTast filter(tastInput, tastOutput, cfg);
    filter.Run();

    if (cfg.random)
    {
        random_tast(&tastOutput[0], &tastOutput[tastOutput.size()]);
    }
}

void random_tast(TastEntry* begin, TastEntry* end)
{
    if (end - begin > MINLIST_TO_RANDOM)
    {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(begin, end, generator);
    }
}

} /* end of namespace tast */ 

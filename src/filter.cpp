#include "filter.h"
#include "parallel.h"

#include <random>
#include <algorithm>

namespace tast
{
    
/// Functor to filter input to output by config.
struct CFilterTast
{
    const TastMap& input;           //< input test case collection
    std::vector<TastEntry>& output; //< output test case list
    const CTastConfig& config;      //< the parsed config from cli
    std::set<std::string> keys;     //< alread outputed test case name

    std::vector<std::string> subs;  //< arguments to subtract cases -xxx
    bool optAll = false;            //< --all
    bool optNoTool = false;         //< --notool

    CFilterTast(const TastMap& pool, std::vector<TastEntry>& filter, const CTastConfig& config_in)
        : input(pool), output(filter), config(config_in)
    {}

    static
    bool MatchWild(const std::string& name, const std::string& arg, char wild)
    {
        if (wild == '%')
        {
            return name.find(arg) != std::string::npos;
        }
        else if (wild == '^')
        {
            return name.find(arg) == 0;
        }
        else if (wild == '$')
        {
            return name.rfind(arg) == name.size() - arg.size();
        }
        return false;
    }

    static
    bool MatchArg(const std::string& arg, const TastEntry& item)
    {
        // full match name
        const std::string& name = item.first;
        if (name == arg)
        {
            return true;
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
            return (item.second != nullptr
                    && item.second->m_file.find(file) != std::string::npos
                    && item.second->m_line >= line);
        }

        // match wild ^$*
        size_t iend = arg.size() - 1;
        if (arg.size() == 1)
        {
            return MatchWild(name, arg, '^');
        }
        else if (arg[0] == '^')
        {
            return MatchWild(name, arg.substr(1), '^');
        }
        else if (arg[iend] == '*')
        {
            return MatchWild(name, arg.substr(0, iend), '^');
        }
        else if (arg[iend] == '$')
        {
            return MatchWild(name, arg.substr(0, iend), '$');
        }
        else if (arg[0] == '*')
        {
            return MatchWild(name, arg.substr(1), '$');
        }
        else
        {
            return MatchWild(name, arg, '%');
        }

        return false;
    }

    bool MatchSub(const TastEntry& item)
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

    void AddTast(const TastEntry& item)
    {
        if (item.second == nullptr)
        {
            return;
        }
        if (keys.count(item.first) > 0)
        {
            return;
        }
        if (config.notool && item.second->m_autoRun == false)
        {
            return;
        }
        if (!MatchSub(item))
        {
            output.push_back(item);
            keys.insert(item.first);
        }
    }

    void AddAll()
    {
        for (auto& item : input)
        {
            if (item.second != nullptr && (item.second->m_autoRun || config.all))
            {
                AddTast(item);
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

        auto it = input.find(arg);
        if (it != input.end())
        {
            // only add full match if possible
            return AddTast(*it);
        }

        for (auto& item : input)
        {
            if (MatchArg(arg, item))
            {
                AddTast(item);
            }
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

void filter_tast(const TastMap& tastInput, TastList& tastOutput, const CTinyIni& cli)
{
    CTastConfig cfg;
    cfg.ParseCli(cli);
    return filter_tast(tastInput, tastOutput, cfg);
}

void filter_tast(const TastMap& tastInput, TastList& tastOutput, const CTastConfig& cfg)
{
    if (tastInput.empty())
    {
        return;
    }

    CFilterTast filter(tastInput, tastOutput, cfg);
    filter.Run();

    if (cfg.random && tastOutput.size() > 2)
    {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(tastOutput.begin(), tastOutput.end(), generator);
    }
}
} /* end of namespace tast */ 

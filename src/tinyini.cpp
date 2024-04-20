#include "tinyini.h"

#include <fstream>

#include <ctype.h>

namespace util {

// Trim space in both ends in-place, return the count of chars removed.
int Trim(std::string& src)
{
    size_t size = src.size();
    size_t begin = 0;
    while (begin < src.size() && ::isspace(src[begin]))
    {
        begin++;
    }
    if (begin == size)
    {
        src.clear();
        return begin;
    }

    size_t end = size - 1;
    while (end > begin && ::isspace(src[end]))
    {
        end--;
    }
    if (begin > 0 || end < size - 1)
    {
        src = src.substr(begin, end - begin + 1);
    }
    return size - src.size();
}

} // end of namespace util

namespace tast
{

void CTinyIni::ParseLines(const std::vector<std::string>& args)
{
    if (args.empty())
    {
        return;
    }
    std::vector<const char*> argv;
    argv.push_back(""); // argv[0] as program name
    for (size_t i = 0; i < args.size(); ++i)
    {
        argv.push_back(const_cast<char*>(args[i].c_str()));
    }
    return ParseCli((int)argv.size(), const_cast<char**>(&argv[0]));
}

bool CTinyIni::LoadIni(const std::string& file)
{
    std::ifstream fin(file);
    if (!fin)
    {
        return false;
    }

    std::vector<std::string> args;
    std::string section;
    std::string line;
    while (std::getline(fin, line))
    {
        util::Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';')
        {
            continue;
        }

        if (line == "--" || line == "[--]")
        {
            section.clear();
            continue;
        }

        if (line.size() > 2 && line.front() == '[' && line.back() == ']')
        {
            section = line.substr(1, line.size()-2);
            continue;
        }

        size_t equal = line.find('=');
        if (equal != std::string::npos)
        {
            std::string key = line.substr(0, equal);
            std::string val = line.substr(equal + 1);
            util::Trim(key);
            util::Trim(val);
            if (!section.empty())
            {
                std::string tmp = section;
                tmp.append(1, '.').append(key);
                key.swap(tmp);
            }
            std::string tmp = key;
            tmp.append(1, '=').append(val);
            line.swap(tmp);
        }

        args.push_back(line);
    }

    ParseLines(args);
    return true;
}

void CTinyIni::Merge(CTinyIni& that)
{
    for (auto& item: that.m_mapOption)
    {
        this->m_mapOption.insert(item);
    }
    for (auto& item: that.m_vecArg)
    {
        this->m_vecArg.push_back(item);
    }
}

} // end of namespace tast


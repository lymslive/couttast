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

bool CTinyCliPtr::GetValue(std::string& val, const std::string& key)
{
    if (m_ptr == nullptr)
    {
        return false;
    }
    auto it = m_ptr->m_mapOption.find(key);
    if (it != m_ptr->m_mapOption.end())
    {
        val = it->second;
        return true;
    }
    return false;
}

bool CTinyCliPtr::GetValue(std::string& val, const char* kp1, const char* kp2)
{
    std::string key(kp1);
    key.append(1, '.').append(kp2);
    return GetValue(val, key);
}

bool CTinyCliPtr::GetValue(int& val, const std::string& key)
{
    std::string str;
    if (GetValue(str, key))
    {
        val = atoi(str.c_str());
        return true;
    }
	return false;
}

bool CTinyCliPtr::GetValue(int& val, const char* kp1, const char* kp2)
{
    std::string key(kp1);
    key.append(1, '.').append(kp2);
    return GetValue(val, key);
}

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

std::ostream& operator<<(std::ostream& os, const tast::CTinyCli& cli)
{
    os << "...\n";
    os << "option: " << cli.m_mapOption << "\n";
    os << "argument: " << cli.m_vecArg;
    return os;
}

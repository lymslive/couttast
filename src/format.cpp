#include "format.h"

#include <string.h>

namespace tast
{

/* ---------------------------------------------------------------------- */

void CTastConfig::ParseCli(const CTinyCli& cli)
{
    for (auto& opt : cli.m_mapOption)
    {
        if (opt.first == "help" || opt.first == "h")
        {
            this->help = 'h';
        }
        if (opt.first == "list" || opt.first == "l")
        {
            this->list = 'l';
        }
        else if (opt.first == "List" || opt.first == "L")
        {
            this->list = 'L';
        }
        else if (opt.first == "cout")
        {
            this->cout = opt.second;
        }
        else if (opt.first.size() > 1 && opt.second.size() == 1 && opt.second[0] == '-')
        {
            this->subs.push_back(opt.first);
        }
        else if (opt.first == "all")
        {
            this->all = true;
        }
        else if (opt.first == "notool")
        {
            this->notool = true;
        }
        else if (opt.first == "job")
        {
            this->job = atoi(opt.second.c_str());
        }
        else if (opt.first == "colour")
        {
            if (opt.second == "always")
            {
                this->colour = 'a';
            }
            else
            {
                this->colour = 'c';
            }
        }
        else if (opt.first == "nocolour")
        {
            this->colour = 'n';
        }
    }

    if (this->all && this->notool)
    {
        this->notool = false;
    }

    // --cout=silent imply --nocolour
    // default imply --colour
    // --colour=alway ingore terminal support colour check.
    if (this->colour == 0 && this->cout == "silent")
    {
        this->colour = 'n';
    }

    for (auto& arg : cli.m_vecArg)
    {
        if (arg[0] == '+' && arg.size() > 1)
        {
            this->adds.push_back(arg.substr(1));
        }
        else
        {
            this->adds.push_back(arg);
        }
    }
}

/* ---------------------------------------------------------------------- */

int CTastFoot::ReadLine(const char* psz, size_t len)
{
    if (psz == nullptr || len < 9)
    {
        return 0;
    }

    // [PASS] or [FIAL]
    const char* head = psz;
    if (strncmp(head, "<< [PASS] ", 10) == 0)
    {
        this->failed = false;
    }
    else if (strncmp(head, "<< [FAIL] ", 10) == 0)
    {
        this->failed = true;
    }
    else
    {
        return 0;
    }

    // failed count: one number
    head += 10;
    int errors = 0;
    while (*head != ' ' && *head != '\0')
    {
        if (*head >= '0' && *head <= '9')
        {
            errors = errors * 10 + (*head - '0');
        }
        else
        {
            return -1;
        }
        head++;
    }
    if (errors > 0)
    {
        this->count = errors;
        if (!this->failed)
        {
            return 2;
        }
    }

    if (*head == '\0')
    {
        // summary foot, only two fields
        return 2;
    }

    // test name
    head++;
    while (*head != ' ' && *head != '\0')
    {
        this->name.push_back(*head);
        head++;
    }

    if (*head == '\0')
    {
        return -3;
    }

    // runtim is micro-seconds
    head++;
    int64_t us = 0;
    while (*head != ' ' && *head != '\0')
    {
        if (*head >= '0' && *head <= '9')
        {
            us = us * 10 + (*head - '0');
        }
        else
        {
            return -3;
        }
        head++;
    }
    this->runtime = us;

    // runtime unit in the end
    if (strncmp(head, " us", 3) == 0)
    {
        return 4;
    }
    else
    {
        return -4;
    }
}

/* ---------------------------------------------------------------------- */

void CTastSummary::Print(std::string& output) const
{
    output.append("## Summary\n");
    output.append("<< [PASS] ").append(std::to_string(this->pass)).append("\n");
    output.append("<< [FAIL] ").append(std::to_string(this->fail));
    for (size_t i = 0; i < this->failNames.size(); ++i)
    {
        if (i > 0)
        {
            output.append("\n");
        }
        output.append("!! ").append(this->failNames[i]);
    }
}

} // end of namespace tast

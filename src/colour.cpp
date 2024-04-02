#include "colour.h"
#include "format.h"

#include <string.h>
#include <stdio.h>

namespace tast
{

enum TerminalColour
{
    TC_NORMAL = 0,
    TC_BLACK = 30,
    TC_RED = 31,
    TC_GREEN = 32,
    TC_YELLOW = 33,
    TC_BLUE = 34,
    TC_MAGENTA = 35,
    TC_CYAN = 36,
    TC_WHITE = 37,
    TC_GREY = 90,
};

struct ColourPrinter
{
    int colour = 0;

    ColourPrinter(int colour_in) : colour(colour_in)
    {
    }

    void Print(const char* str)
    {
        if (colour != TC_NORMAL)
        {
            printf("\x1b[%dm%s\x1b[0m", colour, str);
        }
        else
        {
            printf("%s", str);
        }
    }

    void Println(const char* str)
    {
        Print(str);
        printf("\n");
    }

    /// Print string slice [first, last) by tmp modify *last to '\0'.
    /// Return last for next slice.
    const char* Print(const char* first, const char* last)
    {
        char lastChar = *last;
        char* lastMut = const_cast<char*>(last);
        *lastMut = '\0';
        Print(first);
        *lastMut = lastChar;
        return last;
    }

    const char* Print(const char* first, int count)
    {
        return Print(first, first + count);
    }
};

struct CTastPrinter
{
    // two leading chars, like ## || << ect, also include the next space.
    const char* PrintLead(const char* message, int count = 3)
    {
        return ColourPrinter(TC_GREY).Print(message, count);
    }

    void PrintHead(const char* message)
    {
        ColourPrinter(TC_MAGENTA).Println(message);
    }

    void PrintDesc(const char* message)
    {
        ColourPrinter(TC_YELLOW).Println(message);
    }

    const char* PrintRuntime(const char* first, const char* last)
    {
        if (first + 9 < last)
        {
            first = ColourPrinter(TC_RED).Print(first, last - 9);
        }
        if (first + 6 < last)
        {
            first = ColourPrinter(TC_YELLOW).Print(first, last - 6);
        }
        if (first + 3 < last)
        {
            first = ColourPrinter(TC_GREEN).Print(first, last - 3);
        }
        return ColourPrinter(TC_BLUE).Print(first, last);
    }

    void PrintFoot(const char* message)
    {
        do
        {
            // token 1: static PASS or FAIL
            if (nullptr != ::strstr(message, "[PASS]"))
            {
                message = ColourPrinter(TC_GREEN).Print(message, ::strlen("[PASS]"));
            }
            else if (nullptr != ::strstr(message, "[FAIL]"))
            {
                message = ColourPrinter(TC_RED).Print(message, ::strlen("[FAIL]"));
            }
            else
            {
                fprintf(stderr, "?? invalid foot format");
                break;
            }

            // token 2: count number of fail or pass
            const char* next = ::strchr(message + 1, ' ');
            if (next == nullptr)
            {
                break;
            }
            message = ColourPrinter(TC_NORMAL).Print(message, next);

            // token 3: test name
            next = ::strchr(message + 1, ' ');
            if (next == nullptr)
            {
                break;
            }
            message = ColourPrinter(TC_MAGENTA).Print(message, next);

            // token 4: runtime number
            next = ::strchr(message + 1, ' ');
            if (next == nullptr)
            {
                break;
            }
            // diff colour every three digit
            // message = ColourPrinter(TC_NORMAL).Print(message, next);
            message = PrintRuntime(message, next);
        } while(0);

        // token 5: static us unit in the end
        printf("%s\n", message);
        return;
    }

    void PrintStatement(const char* message)
    {
        const char* mark = ::strstr(message, " [OK]");
        if (mark != nullptr)
        {
            message = ColourPrinter(TC_NORMAL).Print(message, mark);
            ColourPrinter(TC_GREEN).Println(message);
            return;
        }

        mark = strstr(message, " [NO]");
        if (mark != nullptr)
        {
            message = ColourPrinter(TC_NORMAL).Print(message, mark);
            ColourPrinter(TC_RED).Println(message);
            return;
        }

        printf("%s\n", message);
    }

    void PrintExpect(const char* message)
    {
        const char* pszExpect = "Expect: ";
        const char* pszLocation = "Location: ";
        static int lenExpect = strlen(pszExpect);
        static int lenLocation = strlen(pszLocation);

        if (strncmp(message, pszExpect, lenExpect) == 0)
        {
            message = ColourPrinter(TC_NORMAL).Print(message, lenExpect);
            ColourPrinter(TC_GREEN).Println(message);
        }
        else if (strncmp(message, pszLocation, lenLocation) == 0)
        {
            message = ColourPrinter(TC_NORMAL).Print(message, lenLocation);
            ColourPrinter(TC_YELLOW).Println(message);
        }
        else
        {
            // should never reach here
            printf("%s\n", message);
        }
    }

    void PrintFailed(const char* message)
    {
        ColourPrinter(TC_RED).Println(message);
    }

    void PrintLine(const char* message)
    {
        if (*message == '\0')
        {
            printf("\n");
            return;
        }

        if (strncmp(message, "## ", 3) == 0)
        {
            message = PrintLead(message, 3);
            PrintHead(message);
        }
        else if (strncmp(message, "-- ", 3) == 0)
        {
            message = PrintLead(message, 3);
            PrintDesc(message);
        }
        else if (strncmp(message, "<< ", 3) == 0)
        {
            // foot or summary PASS|FAIL
            message = PrintLead(message, 3);
            PrintFoot(message);
        }
        else if (strncmp(message, "|| ", 3) == 0)
        {
            message = PrintLead(message, 3);
            PrintStatement(message);
        }
        else if (strncmp(message, ">> ", 3) == 0)
        {
            // failed statement additional expect value and location
            message = PrintLead(message, 3);
            PrintExpect(message);
        }
        else if (strncmp(message, "!! ", 3) == 0)
        {
            message = PrintLead(message, 3);
            PrintFailed(message);
        }
        else
        {
            printf("%s\n", message);
        }
    }
};

/* ---------------------------------------------------------------------- */

void colour_print(const char* message)
{
    if (message == nullptr)
    {
        return;
    }

    CTastPrinter st;

    char* head = const_cast<char*>(message);
    char* next = strchr(head, '\n');
    while (next != nullptr)
    {
        *next = '\0';
        st.PrintLine(head);
        *next = '\n';
        next++;
        head = next;
        next = strchr(head, '\n');
    }
    st.PrintLine(head);

    fflush(stdout);
}

bool colour_support()
{
    return isatty(STDOUT_FILENO);
}

} // end of namespace tast
